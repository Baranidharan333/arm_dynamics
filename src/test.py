#!/usr/bin/env python3
"""Gravity compensation for a single DAMIAO motor using a URDF model.

Each cycle the motor position is read, the URDF model gives the torque gravity puts on the
joint, and the motor is driven in MIT mode with
    kp = 0, kd = damping, t_ff = clamp(ramp * gravity_scale * tau_g)
so the arm holds its own weight and can be moved freely by hand.

Joint/motor relation:
    q_joint   = direction * (q_motor - zero_offset)
    tau_motor = direction * tau_joint

Typical use:
    python3 test.py --selftest                 # model only, no CAN
    python3 test.py --read-limits              # print the motor's PMAX/VMAX/TMAX registers
    python3 test.py --dry-run --calib-hanging  # support the arm: zero torque, prints q / tau
    python3 test.py --calib-hanging            # run compensation
    python3 test.py --live --calib-hanging     # show the arm following live feedback in a browser
"""

from __future__ import annotations

import argparse
import json
import mimetypes
import math
import signal
import struct
import sys
import threading
import time
import xml.etree.ElementTree as ET
import webbrowser
from dataclasses import dataclass, replace
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

import numpy as np

from drive import DamiaoCanDriver, DriverConfig, ErrorCode
from drive.protocol import MappingRange

HERE = Path(__file__).resolve().parent
DEFAULT_URDF = HERE / "urdf" / "single_joint_1kg.urdf"

# Damiao limit registers used for the MIT/feedback scaling (float32).
RID_PMAX = 0x15
RID_VMAX = 0x16
RID_TMAX = 0x17


# ---------------------------------------------------------------------------
# URDF gravity model
# ---------------------------------------------------------------------------


def _vec(text: str | None, default=(0.0, 0.0, 0.0)) -> np.ndarray:
    return np.array([float(v) for v in text.split()]) if text else np.array(default, float)


def _rpy_to_matrix(rpy: np.ndarray) -> np.ndarray:
    r, p, y = rpy
    cr, sr, cp, sp, cy, sy = math.cos(r), math.sin(r), math.cos(p), math.sin(p), math.cos(y), math.sin(y)
    return np.array(
        [
            [cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr],
            [sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr],
            [-sp, cp * sr, cp * cr],
        ]
    )


def _axis_angle(axis: np.ndarray, angle: float) -> np.ndarray:
    x, y, z = axis
    c, s = math.cos(angle), math.sin(angle)
    C = 1.0 - c
    return np.array(
        [
            [c + x * x * C, x * y * C - z * s, x * z * C + y * s],
            [y * x * C + z * s, c + y * y * C, y * z * C - x * s],
            [z * x * C - y * s, z * y * C + x * s, c + z * z * C],
        ]
    )


def _transform(R: np.ndarray, p: np.ndarray) -> np.ndarray:
    T = np.eye(4)
    T[:3, :3] = R
    T[:3, 3] = p
    return T


def _output_scale(value: str) -> float:
    """Parse a direct scale (``0.9``) or an input:output ratio (``1:0.9``)."""
    try:
        if ":" not in value:
            scale = float(value)
        else:
            input_value, output_value = (float(part.strip()) for part in value.split(":", 1))
            if input_value == 0.0:
                raise ValueError("input side of a ratio cannot be zero")
            scale = output_value / input_value
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "output scale must be a number (0.9) or ratio (1:0.9)"
        ) from exc
    if not math.isfinite(scale) or scale < 0.0:
        raise argparse.ArgumentTypeError("output scale must be finite and non-negative")
    return scale


@dataclass
class _Joint:
    name: str
    type: str
    parent: str
    child: str
    origin: np.ndarray  # 4x4 parent -> joint frame
    axis: np.ndarray


@dataclass
class _Inertial:
    mass: float
    com: np.ndarray  # COM in link frame


class GravityModel:
    """Gravity torques for a serial chain (revolute/continuous + fixed joints) from a URDF."""

    def __init__(self, urdf_path: Path, root: str, tip: str, gravity=(0.0, 0.0, -9.81)) -> None:
        tree = ET.parse(urdf_path).getroot()
        self.gravity = np.array(gravity, float)

        self.inertials: dict[str, _Inertial] = {}
        for link in tree.findall("link"):
            inertial = link.find("inertial")
            if inertial is None:
                continue
            origin = inertial.find("origin")
            com = _vec(origin.get("xyz") if origin is not None else None)
            mass = float(inertial.find("mass").get("value"))
            self.inertials[link.get("name")] = _Inertial(mass, com)

        joints_by_child: dict[str, _Joint] = {}
        for j in tree.findall("joint"):
            origin = j.find("origin")
            xyz = _vec(origin.get("xyz") if origin is not None else None)
            rpy = _vec(origin.get("rpy") if origin is not None else None)
            axis_el = j.find("axis")
            axis = _vec(axis_el.get("xyz") if axis_el is not None else None, (1.0, 0.0, 0.0))
            joint = _Joint(
                name=j.get("name"),
                type=j.get("type"),
                parent=j.find("parent").get("link"),
                child=j.find("child").get("link"),
                origin=_transform(_rpy_to_matrix(rpy), xyz),
                axis=axis / np.linalg.norm(axis),
            )
            if joint.type not in ("revolute", "continuous", "fixed"):
                raise ValueError(f"Unsupported joint type '{joint.type}' ({joint.name})")
            joints_by_child[joint.child] = joint

        # Walk tip -> root to get the chain, then reverse it.
        chain: list[_Joint] = []
        link = tip
        while link != root:
            if link not in joints_by_child:
                raise ValueError(f"No path from '{root}' to '{tip}' in {urdf_path}")
            chain.append(joints_by_child[link])
            link = chain[-1].parent
        self.chain = list(reversed(chain))
        self.joint_names = [j.name for j in self.chain if j.type != "fixed"]
        self.root = root

    @property
    def dof(self) -> int:
        return len(self.joint_names)

    def gravity_torque(self, q: np.ndarray) -> np.ndarray:
        """Torque each joint must apply to hold the chain still against gravity."""
        # Root link mass never loads a joint, so only links after the first joint are collected.
        T = np.eye(4)
        axes, origins = [], []  # world-frame joint axes and origins
        coms = []  # (mass, world COM, number of joints upstream)

        qi = 0
        for joint in self.chain:
            T = T @ joint.origin
            if joint.type != "fixed":
                axes.append(T[:3, :3] @ joint.axis)
                origins.append(T[:3, 3].copy())
                T = T @ _transform(_axis_angle(joint.axis, q[qi]), np.zeros(3))
                qi += 1
            ine = self.inertials.get(joint.child)
            if ine is not None:
                coms.append((ine.mass, T[:3, :3] @ ine.com + T[:3, 3], len(axes)))

        tau = np.zeros(self.dof)
        for mass, com, n_upstream in coms:
            force = mass * self.gravity
            for i in range(n_upstream):
                # gravity torque about joint i, negated = torque needed to hold it
                tau[i] -= axes[i] @ np.cross(com - origins[i], force)
        return tau


# ---------------------------------------------------------------------------
# Motor helpers
# ---------------------------------------------------------------------------


def _is_register_reply(d: bytes, motor_id: int) -> bool:
    # Register replies echo the motor id (little endian) then the command byte.
    # Feedback byte 0 is (err << 4) | id, so while enabled (err = 1) it never matches.
    return (
        len(d) == 8
        and d[0] == motor_id & 0xFF
        and d[1] == (motor_id >> 8) & 0xFF
        and d[2] in (0x33, 0x55, 0xAA)
    )


def read_float_register(drv: DamiaoCanDriver, rid: int, timeout: float = 0.5) -> float | None:
    drv.read_register(rid)
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        msg = drv.recv(timeout=deadline - time.monotonic())
        if msg is None:
            break
        d = bytes(msg.data)
        if (
            msg.arbitration_id == drv.config.master_id
            and _is_register_reply(d, drv.config.motor_id)
            and d[2] == 0x33
            and d[3] == rid
        ):
            return struct.unpack("<f", d[4:8])[0]
    return None


def read_limits(drv: DamiaoCanDriver) -> MappingRange | None:
    pmax = read_float_register(drv, RID_PMAX)
    vmax = read_float_register(drv, RID_VMAX)
    tmax = read_float_register(drv, RID_TMAX)
    if None in (pmax, vmax, tmax):
        return None
    return MappingRange(-pmax, pmax, -vmax, vmax, -tmax, tmax)


def latest_feedback(drv: DamiaoCanDriver, motor_id: int, timeout: float):
    """Return the newest feedback frame from motor_id, draining anything queued."""
    fb = None
    msg = drv.recv(timeout=timeout)
    while msg is not None:
        d = bytes(msg.data)
        if (
            msg.arbitration_id == drv.config.master_id
            and len(d) == 8
            and not _is_register_reply(d, motor_id)
            and (d[0] & 0x0F) == (motor_id & 0x0F)
        ):
            fb = drv.decode_feedback(msg)
        msg = drv.recv(timeout=0.0)
    return fb


class LiveUrdfViewer:
    """Serve a browser view of the URDF arm, updated by motor feedback."""

    def __init__(self, urdf_path: Path, joint_name: str) -> None:
        self.urdf_dir = urdf_path.resolve().parent
        self.urdf_name = urdf_path.name
        self.state = {"q": 0.0, "tau": 0.0}
        self.lock = threading.Lock()
        state, lock, page, urdf_dir = self.state, self.lock, self._page(joint_name), self.urdf_dir

        class Handler(BaseHTTPRequestHandler):
            def do_GET(self):
                if self.path == "/state":
                    with lock:
                        payload = json.dumps(state).encode("utf-8")
                    self.send_response(200)
                    self.send_header("Content-Type", "application/json")
                    self.send_header("Cache-Control", "no-store")
                elif self.path == "/":
                    payload = page.encode("utf-8")
                    self.send_response(200)
                    self.send_header("Content-Type", "text/html; charset=utf-8")
                elif self.path.startswith("/urdf/"):
                    relative = self.path.removeprefix("/urdf/")
                    asset = (urdf_dir / relative).resolve()
                    if not asset.is_relative_to(urdf_dir) or not asset.is_file():
                        self.send_error(404)
                        return
                    payload = asset.read_bytes()
                    self.send_response(200)
                    self.send_header("Content-Type", mimetypes.guess_type(asset.name)[0] or "application/octet-stream")
                else:
                    self.send_error(404)
                    return
                self.send_header("Content-Length", str(len(payload)))
                self.end_headers()
                self.wfile.write(payload)

            def log_message(self, *_args):
                pass

        try:
            self.server = ThreadingHTTPServer(("127.0.0.1", 8765), Handler)
        except OSError as exc:
            raise RuntimeError(f"Cannot start web UI on http://127.0.0.1:8765: {exc}") from exc
        threading.Thread(target=self.server.serve_forever, daemon=True).start()
        self.url = "http://127.0.0.1:8765/"
        webbrowser.open(self.url)

    def _page(self, joint_name: str) -> str:
        return f'''<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Live URDF</title>
<style>*{{box-sizing:border-box}}body{{margin:0;background:#101722;color:#e7edf5;font:15px system-ui,sans-serif}}
main{{max-width:1100px;margin:auto;padding:28px}}h1{{font-size:22px;margin:0 0 6px}}.sub{{color:#9baabd;margin-bottom:20px}}
.panel{{background:#182331;border:1px solid #2b3a4c;border-radius:14px;padding:16px}}
canvas{{display:block;width:100%;height:min(70vh,650px);background:#111b27;border-radius:9px}}
.stats{{display:flex;gap:28px;padding:16px 4px 4px}}.label{{color:#9baabd;font-size:12px}}
.value{{font-size:22px;font-variant-numeric:tabular-nums}}.live{{color:#7ee0ad}}</style></head><body>
<main><h1>Live URDF viewer</h1><div class="sub"><span class="live">● Live</span> &nbsp;{joint_name} · motor feedback</div>
<div class="panel"><canvas id="view"></canvas><div class="stats">
<div><div class="label">JOINT POSITION</div><div class="value" id="q">—</div></div>
<div><div class="label">GRAVITY TORQUE</div><div class="value" id="tau">—</div></div>
</div></div></main>
<script type="importmap">{{"imports":{{"three":"https://cdn.jsdelivr.net/npm/three@0.170.0/build/three.module.js","urdf-loader":"https://cdn.jsdelivr.net/npm/urdf-loader@0.13.1/+esm"}}}}</script>
<script type="module">
import * as THREE from 'three';import URDFLoader from 'urdf-loader';
const canvas=document.getElementById('view'),renderer=new THREE.WebGLRenderer({{canvas,antialias:true}});
renderer.setPixelRatio(devicePixelRatio);renderer.setClearColor(0x111b27);
const scene=new THREE.Scene();scene.add(new THREE.HemisphereLight(0xffffff,0x34445a,2));
const light=new THREE.DirectionalLight(0xffffff,2.2);light.position.set(1,-2,3);scene.add(light);
const camera=new THREE.PerspectiveCamera(42,1,.001,100);camera.up.set(0,0,1);camera.position.set(.45,-.8,.45);camera.lookAt(.1,0,0);
const grid=new THREE.GridHelper(1,20,0x526174,0x29394a);grid.rotation.x=Math.PI/2;scene.add(grid);
let robot=null;const loader=new URDFLoader();loader.packages={{}};
loader.load('/urdf/{self.urdf_name}',model=>{{robot=model;scene.add(robot);
const bounds=new THREE.Box3().setFromObject(robot),size=bounds.getSize(new THREE.Vector3()),center=bounds.getCenter(new THREE.Vector3());
const span=Math.max(size.x,size.y,size.z,.2);camera.position.set(center.x+span*1.1,center.y-span*1.8,center.z+span*1.2);camera.lookAt(center);
}},undefined,error=>{{const status=document.querySelector('.live');status.textContent='URDF load failed: '+error;status.style.color='#ff8888';}});
function resize(){{const r=canvas.getBoundingClientRect();renderer.setSize(r.width,r.height,false);camera.aspect=r.width/r.height;camera.updateProjectionMatrix()}}
new ResizeObserver(resize).observe(canvas);resize();
async function poll(){{try{{const response=await fetch('/state',{{cache:'no-store'}}),v=await response.json();
if(robot)robot.setJointValue('{joint_name}',v.q);
document.getElementById('q').textContent=(v.q*180/Math.PI).toFixed(2)+'°';document.getElementById('tau').textContent=v.tau.toFixed(3)+' Nm';
}}catch(e){{}}setTimeout(poll,30)}}
function draw(){{requestAnimationFrame(draw);renderer.render(scene,camera)}}draw();poll();
</script></body></html>'''

    def update(self, position: float, torque: float) -> bool:
        with self.lock:
            self.state.update(q=position, tau=torque)
        return True

    def close(self) -> None:
        self.server.shutdown()
        self.server.server_close()


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--urdf", type=Path, default=DEFAULT_URDF)
    ap.add_argument("--root", default="base_link")
    ap.add_argument("--tip", default="load_link")
    ap.add_argument("--interface", default="socketcan", help="python-can interface")
    ap.add_argument("--channel", default="can0")
    ap.add_argument("--bitrate", type=int, default=1_000_000)
    ap.add_argument("--motor-id", type=lambda s: int(s, 0), default=0x01)
    ap.add_argument("--master-id", type=lambda s: int(s, 0), default=0x11,
                    help="CAN id the motor sends feedback on")

    ap.add_argument("--direction", type=int, choices=(1, -1), default=-1,
                    help="-1 if motor positive moves opposite to URDF positive; use +1 if aligned")
    ap.add_argument("--zero-offset", type=float, default=0.0,
                    help="motor position [rad] when the arm is horizontal (q_joint = 0)")
    ap.add_argument("--calib-hanging", action="store_true",
                    help="arm is hanging straight down at start: set zero_offset from it")

    ap.add_argument("--rate", type=float, default=500.0, help="control rate [Hz]")
    ap.add_argument("--output-scale", "--gravity-scale", dest="output_scale", type=_output_scale,
                    default=1.0,
                    help="scale model torque before output: 1:1 = 100%%, 1:0.9 = 90%% (default: 1:1)")
    ap.add_argument("--damping", type=float, default=0.05, help="MIT kd [Nm*s/rad]")
    ap.add_argument("--max-torque", type=float, default=4.0, help="|t_ff| clamp [Nm]")
    ap.add_argument("--ramp", type=float, default=0.0,
                    help="torque fade-in time [s]; the arm sags during it, so hold it or start hanging")
    ap.add_argument("--max-vel", type=float, default=6.0, help="disable above this |vel| [rad/s]")
    ap.add_argument("--tmax", type=float, default=None,
                    help="override motor TMAX if registers cannot be read (DM4340: 28)")

    ap.add_argument("--selftest", action="store_true", help="print model torques, no CAN")
    ap.add_argument("--read-limits", action="store_true", help="read PMAX/VMAX/TMAX and exit")
    ap.add_argument("--dry-run", action="store_true", help="send zero torque, only print q / tau")
    ap.add_argument("--live", action="store_true",
                    help="open a browser UI showing arm pose and gravity torque from live feedback")
    return ap.parse_args()


def selftest(model: GravityModel, output_scale: float) -> None:
    print(f"Chain joints: {model.joint_names}")
    print(f"Output scale: {output_scale:.3f}")
    for deg in (-90, -45, 0, 45, 90):
        q = np.full(model.dof, math.radians(deg))
        tau_g = model.gravity_torque(q)
        print(f"  q = {deg:+4d} deg  ->  tau_g = {tau_g} Nm  "
              f"tau_out = {output_scale * tau_g} Nm")


def main() -> int:
    args = parse_args()
    model = GravityModel(args.urdf, args.root, args.tip)
    if model.dof != 1:
        print(f"[ERROR] This script drives one motor; chain has {model.dof} joints", file=sys.stderr)
        return 1

    if args.selftest:
        selftest(model, args.output_scale)
        return 0

    cfg = DriverConfig(interface=args.interface, channel=args.channel, bitrate=args.bitrate,
                       motor_id=args.motor_id, master_id=args.master_id)
    try:
        drv = DamiaoCanDriver.open(cfg)
    except OSError as e:
        print(f"[ERROR] Cannot open {args.channel}: {e}\n"
              f"        Bring it up: sudo ip link set {args.channel} up type can bitrate {args.bitrate}",
              file=sys.stderr)
        return 1

    running = True

    def stop(*_):
        nonlocal running
        running = False

    signal.signal(signal.SIGINT, stop)
    signal.signal(signal.SIGTERM, stop)

    enabled = False
    viewer = None
    try:
        # The MIT torque scaling must match the motor's TMAX, or every command is scaled wrong.
        mapping = read_limits(drv)
        if mapping is not None:
            print(f"[INFO] Motor limits: PMAX={mapping.p_max:.3f} VMAX={mapping.v_max:.3f} "
                  f"TMAX={mapping.t_max:.3f}")
        if args.read_limits:
            if mapping is None:
                print("[ERROR] No register reply. Check wiring, bitrate, motor id.", file=sys.stderr)
                return 1
            return 0
        if mapping is None:
            if args.tmax is None:
                print("[ERROR] Could not read motor limits. Pass --tmax (DM4340: 28) to continue.",
                      file=sys.stderr)
                return 1
            mapping = replace(MappingRange(), t_min=-args.tmax, t_max=args.tmax)
            print(f"[WARN] Using --tmax={args.tmax} with default P/V ranges")
        drv.config = replace(cfg, mapping=mapping)

        max_torque = min(args.max_torque, mapping.t_max)
        kd = args.damping

        # The motor answers enable with a feedback frame. Use it and start the loop at once:
        # every ms spent at zero torque here lets the arm start falling.
        drv.enable_motor()
        enabled = True
        fb = latest_feedback(drv, args.motor_id, timeout=0.2)
        if fb is None:
            drv.send_mit(0.0, 0.0, 0.0, kd, 0.0)
            fb = latest_feedback(drv, args.motor_id, timeout=0.2)
        if fb is None:
            print("[ERROR] No feedback after enable.", file=sys.stderr)
            return 1
        if fb.err != ErrorCode.ENABLED:
            print(f"[ERROR] Motor error state after enable: {fb.err:#x}", file=sys.stderr)
            return 1

        zero_offset = args.zero_offset
        if args.calib_hanging:
            # Hanging straight down is q_joint = -pi/2.
            zero_offset = fb.pos + args.direction * (math.pi / 2)
            print(f"[INFO] Calibrated from hanging pose: zero_offset = {zero_offset:+.4f} rad")

        if args.live:
            try:
                viewer = LiveUrdfViewer(args.urdf, model.joint_names[0])
                print(f"[INFO] Live URDF web UI: {viewer.url} (Ctrl+C to stop)")
            except (RuntimeError, OSError) as e:
                print(f"[ERROR] Cannot start live URDF view: {e}", file=sys.stderr)
                return 1

        print(f"[INFO] {'DRY RUN (zero torque)' if args.dry_run else 'Running'} at {args.rate:.0f} Hz, "
              f"output_scale={args.output_scale:.3f}, kd={kd}, max_torque={max_torque} Nm. Ctrl+C to stop.")

        period = 1.0 / args.rate
        start = time.monotonic()
        next_tick = start
        last_print = start
        missed = 0
        rc = 0

        while running:
            now = time.monotonic()
            ramp = 1.0 if args.ramp <= 0 else min(1.0, (now - start) / args.ramp)

            q_joint = args.direction * (fb.pos - zero_offset)
            tau_joint = float(model.gravity_torque(np.array([q_joint]))[0])
            if viewer is not None:
                viewer.update(q_joint, tau_joint)
            # 1:1 sends the model torque; 1:0.9 sends 90% of it.
            tau_cmd = args.direction * ramp * args.output_scale * tau_joint
            tau_cmd = max(-max_torque, min(max_torque, tau_cmd))
            if args.dry_run:
                tau_cmd = 0.0

            drv.send_mit(0.0, 0.0, 0.0, kd, tau_cmd)
            new_fb = latest_feedback(drv, args.motor_id, timeout=min(period, 0.01))
            if new_fb is None:
                missed += 1
                if missed > 50:
                    print("[ERROR] Feedback lost.", file=sys.stderr)
                    rc = 1
                    break
            else:
                missed = 0
                fb = new_fb
                if fb.err != ErrorCode.ENABLED:
                    print(f"[ERROR] Motor reported error {fb.err:#x}", file=sys.stderr)
                    rc = 1
                    break
                if abs(fb.vel) > args.max_vel:
                    print(f"[ERROR] Velocity {fb.vel:+.2f} rad/s over --max-vel. Wrong --direction "
                          f"or zero offset?", file=sys.stderr)
                    rc = 1
                    break

            if now - last_print >= 0.2:
                print(f"q={math.degrees(q_joint):+7.2f} deg  tau_g={tau_joint:+.3f} Nm  "
                      f"cmd={tau_cmd:+.3f} Nm  meas={fb.torque:+.3f} Nm  "
                      f"T_mos={fb.t_mos_c}C T_rotor={fb.t_rotor_c}C")
                last_print = now

            next_tick += period
            sleep = next_tick - time.monotonic()
            if sleep > 0:
                time.sleep(sleep)
            else:
                next_tick = time.monotonic()
    finally:
        if viewer is not None:
            viewer.close()
        if enabled:
            print("\n[INFO] Disabling motor (support the arm!)")
            drv.disable_motor()
            time.sleep(0.02)
        drv.close()
    return rc


if __name__ == "__main__":
    sys.exit(main())
