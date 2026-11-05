#!/usr/bin/env python3
"""
plot_server.py
---------------
Generic live web plotter for arbitrary ROS 2 topic fields - a browser-based
stand-in for `rqt_plot` / PlotJuggler that doesn't need a desktop session.

Series are given in the same "/topic/field[index]" path syntax rqt_plot and
PlotJuggler use, e.g.:

    /joint_cmd/position[3]
    /jnt_cmt_to_ctrl/position[10]
    /right_arm_mit_controller/joint_trajectory/points[0]/positions[3]
    /joint_states/position[2]

The topic name isn't known ahead of time from the path alone (topic names can
contain slashes), so it's resolved against the live ROS graph: the longest
currently-advertised topic name that prefixes the path is taken as the topic,
and the remainder is walked as a field path. Message types are looked up and
imported dynamically, so this works for any message type already installed,
not just JointState/JointTrajectory.

Run with:
    source /opt/ros/<distro>/setup.bash
    source install/setup.bash
    ros2 run joint_analyzer plot_server --series "/joint_cmd/position[3]" "/joint_states/position[2]"

Then open http://<host>:<port>/ in a browser.
"""

import argparse
import functools
import logging
import os
import re
import sys
import threading
import time
from dataclasses import dataclass, field
from typing import Any, Deque, List, Optional, Tuple
from collections import deque

# Force CycloneDDS to send user data via unicast rather than multicast - same
# workaround as the rest of joint_analyzer, see ros_interface.py.
os.environ["CYCLONEDDS_URI"] = (
    "<CycloneDDS><Domain><General>"
    "<AllowMulticast>spdp</AllowMulticast>"
    "</General></Domain></CycloneDDS>"
)

import rclpy
from rclpy.executors import SingleThreadedExecutor
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from rosidl_runtime_py.utilities import get_message

from flask import Flask, jsonify, render_template

# Validated dark-surface categorical order (see the dataviz color formula):
# adjacent pairs clear the CVD/contrast gates on this app's dark background,
# in this fixed order. Cycles past 8 series, at which point identity should
# really be split across separate plots instead.
_PALETTE = [
    "#3987e5", "#d95926", "#199e70", "#c98500",
    "#d55181", "#008300", "#9085e9", "#e66767",
]

_SEG_RE = re.compile(r"^([A-Za-z_]\w*)(\[(\d+)\])?$")


def _resolve_field(msg: Any, field_path: str) -> Any:
    obj = msg
    for seg in field_path.split("/"):
        if not seg:
            continue
        m = _SEG_RE.match(seg)
        if not m:
            raise ValueError(f"bad field path segment: {seg!r}")
        name, _, idx = m.groups()
        obj = getattr(obj, name)
        if idx is not None:
            obj = obj[int(idx)]
    return obj


def _find_topic(
    series_path: str, topics: List[Tuple[str, List[str]]]
) -> Optional[Tuple[str, str, str]]:
    """Longest advertised topic name that prefixes series_path, plus the
    remaining field path and the topic's message type string."""
    best: Optional[Tuple[str, str]] = None
    for name, types in topics:
        if series_path == name or series_path.startswith(name + "/"):
            if best is None or len(name) > len(best[0]):
                best = (name, types[0])
    if best is None:
        return None
    topic_name, type_str = best
    field_path = series_path[len(topic_name):].lstrip("/")
    return topic_name, type_str, field_path


@dataclass
class Series:
    path: str
    color: str
    topic: Optional[str] = None
    field_path: Optional[str] = None
    error: Optional[str] = None
    buffer: Deque[Tuple[float, float]] = field(default_factory=deque)


class _PlotNode(Node):
    """Resolves each series' topic/field lazily against the live ROS graph
    (topics may not be advertised yet at startup) and buffers samples."""

    def __init__(self, series_list: List[Series], window_sec: float, lock: threading.Lock):
        super().__init__("topic_plot_node")
        self._series = series_list
        self._window_sec = window_sec
        self._lock = lock
        self._subs: List[Any] = []
        self.create_timer(1.0, self._discover)
        self._discover()

    def _discover(self) -> None:
        pending = [i for i, s in enumerate(self._series) if s.topic is None and s.error is None]
        if not pending:
            return

        topics = self.get_topic_names_and_types()

        for i in pending:
            s = self._series[i]
            found = _find_topic(s.path, topics)
            if found is None:
                continue

            topic_name, type_str, field_path = found
            try:
                msg_class = get_message(type_str)
            except Exception as exc:  # noqa: BLE001
                s.error = f"unknown message type {type_str!r}: {exc}"
                continue

            s.topic = topic_name
            s.field_path = field_path
            sub = self.create_subscription(
                msg_class,
                topic_name,
                functools.partial(self._on_msg, index=i),
                qos_profile_sensor_data,
            )
            self._subs.append(sub)
            self.get_logger().info(
                f"[{s.path}] -> topic={topic_name} type={type_str} field={field_path!r}"
            )

    def _on_msg(self, msg: Any, index: int) -> None:
        s = self._series[index]
        try:
            value = float(_resolve_field(msg, s.field_path))
        except Exception:  # noqa: BLE001
            # Transient (e.g. array not yet sized to this index) - drop the
            # sample rather than latching a permanent error.
            return

        now = time.time()
        cutoff = now - self._window_sec
        with self._lock:
            s.buffer.append((now, value))
            while len(s.buffer) > 1 and s.buffer[0][0] < cutoff:
                s.buffer.popleft()


logging.getLogger("werkzeug").setLevel(logging.WARNING)

app = Flask(__name__)
# Static JS/CSS here are actively edited during development; without this the
# browser can keep serving a stale cached plot.js after a fix, making an
# already-fixed bug look like it's still happening.
app.config["SEND_FILE_MAX_AGE_DEFAULT"] = 0
_lock = threading.Lock()
_series: List[Series] = []


@app.route("/")
def index():
    return render_template("plot.html")


@app.route("/api/series")
def api_series():
    with _lock:
        return jsonify([
            {
                "id": i,
                "path": s.path,
                "color": s.color,
                "topic": s.topic,
                "field": s.field_path,
                "error": s.error,
            }
            for i, s in enumerate(_series)
        ])


@app.route("/api/data")
def api_data():
    with _lock:
        series_out = [
            {
                "t": [p[0] for p in s.buffer],
                "v": [p[1] for p in s.buffer],
                "resolved": s.topic is not None,
            }
            for s in _series
        ]
    return jsonify({"now": time.time(), "series": series_out})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=8131)
    parser.add_argument(
        "--window", type=float, default=5.0,
        help="seconds of history kept/plotted per series")
    parser.add_argument(
        "--series", nargs="+",
        default=[
            "/joint_cmd/position[3]",
            "/jnt_cmt_to_ctrl/position[10]",
            "/right_arm_mit_controller/joint_trajectory/points[0]/positions[3]",
            "/right_arm_mit_controller/debug_command_interfaces/position[3]",
            "/RightArmSystem_hw_write_cmd/position[3]",
            "/RightArmSystem_ordered_joint_states/position[3]",
            "/joint_states/position[2]",
        ],
        help='topic/field paths, e.g. "/joint_states/position[2]"')
    # `ros2 launch`/`ros2 run` append --ros-args -r __node:=... etc. to argv;
    # strip those before argparse sees them, or it errors out on launch.
    args = parser.parse_args(rclpy.utilities.remove_ros_args(sys.argv)[1:])

    global _series
    _series = [
        Series(path=p, color=_PALETTE[i % len(_PALETTE)])
        for i, p in enumerate(args.series)
    ]

    rclpy.init()
    node = _PlotNode(_series, args.window, _lock)
    executor = SingleThreadedExecutor()
    executor.add_node(node)

    spin_thread = threading.Thread(
        target=executor.spin, daemon=True, name="ros2_executor")
    spin_thread.start()

    try:
        app.run(host=args.host, port=args.port, threaded=True)
    finally:
        executor.shutdown(timeout_sec=1.0)
        node.destroy_node()
        # rclpy installs its own SIGINT handler that may already have shut
        # the context down by the time we get here (Ctrl-C races this
        # finally block against that handler) — that's expected, not an
        # error, so swallow it rather than let a benign double-shutdown
        # print a traceback on every normal Ctrl-C.
        try:
            if rclpy.ok():
                rclpy.shutdown()
        except Exception:
            pass


if __name__ == "__main__":
    main()
