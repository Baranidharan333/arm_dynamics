#!/usr/bin/env python3
"""
server.py
---------
Web backend for the Joint Command vs Joint State Analyzer.

Replaces the old PyQt6 desktop UI (gui.py / main.py, removed) with a Flask
app: this process owns the single RosInterface/Recorder/DiffStats instance
and the ROS executor thread, same as the old MainWindow did; the browser
just polls a small JSON API instead of using Qt signals/timers.

Run with:
    source /opt/ros/<distro>/setup.bash
    source install/setup.bash
    ros2 run joint_analyzer server [--host 0.0.0.0] [--port 8130]
  or, to launch server + plot_server together:
    ros2 launch joint_analyzer joint_analyzer.launch.py

Then open http://<host>:<port>/ in a browser.
"""

import argparse
import logging
import math
import os
import sys
import tempfile
import threading
import time
from typing import List, Optional

import rclpy
from flask import Flask, jsonify, request, render_template, send_file

from joint_analyzer.recorder import Recorder
from joint_analyzer.ros_interface import RosInterface
from joint_analyzer.utils import (
    THRESHOLD_GREEN, THRESHOLD_YELLOW,
    DiffStats, default_csv_filename, format_duration,
)

logging.getLogger("werkzeug").setLevel(logging.WARNING)

app = Flask(__name__)

_lock = threading.Lock()

_ros = RosInterface()
_recorder = Recorder()
_stats = DiffStats()

_state = {
    "connected": False,
    "cmd_topic": "/jnt_cmt_to_ctrl",
    "state_topic": "/joint_states",
    "error": None,
}

# Written by the ROS callback thread, read (and "ticked" into recorder/stats)
# by GET /api/data - mirrors the old app's 30 Hz QTimer decoupling ROS
# publish rate from UI refresh rate, except here the polling browser sets
# the tick rate.
_latest = {
    "names": [],
    "cmd": [],
    "actual": [],
    "cmd_live": False,
    "actual_live": False,
}


def _on_data(names: List[str], cmd: List[Optional[float]], actual: List[Optional[float]],
             cmd_live: bool, actual_live: bool) -> None:
    with _lock:
        _latest["names"] = names
        _latest["cmd"] = cmd
        _latest["actual"] = actual
        _latest["cmd_live"] = cmd_live
        _latest["actual_live"] = actual_live


def _on_error(msg: str) -> None:
    with _lock:
        _state["error"] = msg


def _diff_bucket(diff: Optional[float]) -> Optional[str]:
    if diff is None:
        return None
    if diff < THRESHOLD_GREEN:
        return "green"
    if diff < THRESHOLD_YELLOW:
        return "yellow"
    return "red"


def _status_state() -> str:
    if not _state["connected"]:
        return "disconnected"
    if _state["error"]:
        return "error"
    cmd_live = _latest["cmd_live"]
    actual_live = _latest["actual_live"]
    if not cmd_live and not actual_live:
        return "waiting"
    if not actual_live:
        return "cmd_only"
    if not cmd_live:
        return "actual_only"
    return "connected"


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/topics")
def api_topics():
    return jsonify({"topics": RosInterface.get_joint_state_topics()})


@app.route("/api/topic_status")
def api_topic_status():
    topics = [t for t in request.args.get("topics", "").split(",") if t]
    return jsonify(RosInterface.check_topics_publishing(topics))


@app.route("/api/domain", methods=["POST"])
def api_domain():
    with _lock:
        if _state["connected"]:
            return jsonify({"ok": False, "error": "Disconnect first before changing the Domain ID."}), 400
        domain_id = int(request.json.get("domain_id", 0))
        RosInterface.set_domain_id(domain_id)
    return jsonify({"ok": True, "domain_id": domain_id})


@app.route("/api/domain", methods=["GET"])
def api_domain_get():
    return jsonify({"domain_id": int(os.environ.get("ROS_DOMAIN_ID", "0"))})


@app.route("/api/connect", methods=["POST"])
def api_connect():
    body = request.json or {}
    cmd_topic = (body.get("cmd_topic") or "").strip()
    state_topic = (body.get("state_topic") or "").strip()

    if not cmd_topic or not state_topic:
        return jsonify({"ok": False, "error": "Please select both topics before connecting."}), 400

    with _lock:
        _state["error"] = None
        _latest["names"] = []
        _latest["cmd"] = []
        _latest["actual"] = []
        _latest["cmd_live"] = False
        _latest["actual_live"] = False
        _stats.reset()

        _ros.connect(
            follower_cmd_topic=cmd_topic,
            follower_state_topic=state_topic,
            on_data=_on_data,
            on_error=_on_error,
        )
        _state["connected"] = True
        _state["cmd_topic"] = cmd_topic
        _state["state_topic"] = state_topic

    return jsonify({"ok": True})


@app.route("/api/disconnect", methods=["POST"])
def api_disconnect():
    with _lock:
        _ros.disconnect()
        _state["connected"] = False
        _latest["names"] = []
        _latest["cmd"] = []
        _latest["actual"] = []
        _latest["cmd_live"] = False
        _latest["actual_live"] = False
        if _recorder.is_recording:
            _recorder.stop()
    return jsonify({"ok": True})


@app.route("/api/data")
def api_data():
    use_deg = request.args.get("degrees", "true").lower() != "false"
    mult = 180.0 / math.pi if use_deg else 1.0

    with _lock:
        names = list(_latest["names"])
        cmd = list(_latest["cmd"])
        actual = list(_latest["actual"])
        cmd_live = _latest["cmd_live"]
        actual_live = _latest["actual_live"]
        connected = _state["connected"]

        diffs = [
            abs(c - a) if c is not None and a is not None else None
            for c, a in zip(cmd, actual)
        ]

        # One "tick": mirrors the old 30 Hz QTimer driving recorder/stats
        # off the latest cached ROS sample, except the poll rate IS the
        # tick rate here.
        if connected:
            _recorder.update(names, cmd, actual)
            if cmd_live:
                valid = [d for d in diffs if d is not None]
                if valid:
                    _stats.update(names, valid)

        rows = []
        for name, c, a, d in zip(names, cmd, actual, diffs):
            rows.append({
                "name": name,
                "cmd": (c * mult) if (cmd_live and c is not None) else None,
                "actual": (a * mult) if (actual_live and a is not None) else None,
                "diff": (d * mult) if (cmd_live and actual_live and d is not None) else None,
                "diff_bucket": _diff_bucket(d) if (cmd_live and actual_live) else None,
            })

        stats_samples = _stats.samples
        stats_avg = _stats.avg * mult
        stats_max = _stats.maximum * mult

        payload = {
            "connected": connected,
            "status": _status_state(),
            "error": _state["error"],
            "cmd_live": cmd_live,
            "actual_live": actual_live,
            "unit": "deg" if use_deg else "rad",
            "rows": rows,
            "stats": {"avg": stats_avg, "max": stats_max, "samples": stats_samples},
            "recording": {
                "active": _recorder.is_recording,
                "samples": _recorder.sample_count,
                "elapsed": _recorder.elapsed_seconds,
                "elapsed_str": format_duration(_recorder.elapsed_seconds),
            },
        }
    return jsonify(payload)


@app.route("/api/stats/reset", methods=["POST"])
def api_stats_reset():
    with _lock:
        _stats.reset()
    return jsonify({"ok": True})


@app.route("/api/record/start", methods=["POST"])
def api_record_start():
    with _lock:
        if not _state["connected"]:
            return jsonify({"ok": False, "error": "Connect to topics before recording."}), 400
        _recorder.start()
    return jsonify({"ok": True})


@app.route("/api/record/stop", methods=["POST"])
def api_record_stop():
    with _lock:
        _recorder.stop()
    return jsonify({"ok": True})


@app.route("/api/record/clear", methods=["POST"])
def api_record_clear():
    with _lock:
        if _recorder.is_recording:
            _recorder.stop()
        _recorder.clear()
    return jsonify({"ok": True})


@app.route("/api/record/csv")
def api_record_csv():
    use_deg = request.args.get("degrees", "true").lower() != "false"

    with _lock:
        if _recorder.sample_count == 0:
            return jsonify({"ok": False, "error": "No samples recorded yet."}), 400

        fd, tmp_path = tempfile.mkstemp(suffix=".csv")
        os.close(fd)
        _recorder.save_csv(tmp_path, use_degrees=use_deg)

    filename = default_csv_filename()
    response = send_file(tmp_path, as_attachment=True, download_name=filename, mimetype="text/csv")
    # Best-effort cleanup once Flask has streamed the file to the client.
    response.call_on_close(lambda: os.path.exists(tmp_path) and os.remove(tmp_path))
    return response


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=8130)
    # `ros2 launch`/`ros2 run` append --ros-args -r __node:=... etc. to argv;
    # strip those before argparse sees them, or it errors out on launch.
    args = parser.parse_args(rclpy.utilities.remove_ros_args(sys.argv)[1:])

    try:
        app.run(host=args.host, port=args.port, threaded=True)
    finally:
        _ros.shutdown()


if __name__ == "__main__":
    main()
