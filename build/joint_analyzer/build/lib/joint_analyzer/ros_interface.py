"""
ros_interface.py
----------------
Handles all ROS 2 communication for the joint analyzer.

Architecture (2-topic):
  - follower_cmd_topic  → JointState (e.g. /joint_cmd)
  - follower_state_topic→ JointState (e.g. /joint_states)
"""

import os
import re
import threading
from dataclasses import dataclass
from typing import Callable, Dict, List, Optional, Tuple

# Force CycloneDDS to send user data via unicast rather than multicast.
# This prevents data streams (like /joint_cmd) from being blocked by the network switch.
os.environ["CYCLONEDDS_URI"] = (
    "<CycloneDDS><Domain><General>"
    "<AllowMulticast>spdp</AllowMulticast>"
    "</General></Domain></CycloneDDS>"
)

import rclpy
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import JointState


@dataclass
class MatchedJoint:
    name: str           # Display name, as published on the bus (e.g. 'left_joint_1')
    side: int           # 0 = left, 1 = right, 2 = other
    jtype: int          # 0 = regular, 1 = finger/gripper
    num: int            # Joint number for sorting
    cmd: Optional[float] = None
    actual: Optional[float] = None


class _AnalyzerNode(Node):
    """
    Subscribes to Follower Command and Follower State JointState topics.
    """

    def __init__(
        self,
        follower_cmd_topic:   str,
        follower_state_topic:  str,
        on_data:              Callable[[List[str], List[Optional[float]], List[Optional[float]], bool, bool], None],
        on_error:             Callable[[str], None],
    ) -> None:
        super().__init__("joint_analyzer_node")

        self._on_data  = on_data
        self._on_error = on_error

        # Cache of matched joints keyed by (side, suffix)
        self._matched_joints: Dict[Tuple[int, str], MatchedJoint] = {}
        self._cmd_live = False
        self._actual_live = False

        # Callback group to prevent execution starvation
        cb_group = ReentrantCallbackGroup()

        # Subscribe to both topics
        self._sub_follower_cmd = self.create_subscription(JointState, follower_cmd_topic, self._follower_cmd_callback, qos_profile_sensor_data, callback_group=cb_group)
        self._sub_follower_state = self.create_subscription(JointState, follower_state_topic, self._follower_state_callback, qos_profile_sensor_data, callback_group=cb_group)

    # ── Callbacks ─────────────────────────────────────────────────────────────

    def _follower_cmd_callback(self, msg: JointState) -> None:
        names = list(msg.name)
        pos   = list(msg.position)
        if names and len(names) == len(pos):
            self._cmd_live = True
            self._update_joint_values(names, pos, "cmd")

    def _follower_state_callback(self, msg: JointState) -> None:
        names = list(msg.name)
        pos   = list(msg.position)
        if names and len(names) == len(pos):
            self._actual_live = True
            self._update_joint_values(names, pos, "actual")

    # ── Helpers ───────────────────────────────────────────────────────────────

    def _update_joint_values(self, names: List[str], positions: List[float], field_name: str, default_side: Optional[int] = None) -> None:
        """Update values for the specified field and emit the sorted dataset."""
        for name, pos in zip(names, positions):
            side, suffix = self._normalize_name(name, default_side=default_side)
            key = (side, suffix)

            if key not in self._matched_joints:
                if "finger" in suffix or "gripper" in suffix or "thumb" in suffix:
                    jtype = 1
                else:
                    jtype = 0
                match = re.search(r'\d+', suffix)
                num = int(match.group()) if match else 0

                # Display the real name as published on the bus (e.g.
                # "left_joint_1") rather than inventing a "daksha_" prefix
                # this robot's topics don't actually use.
                self._matched_joints[key] = MatchedJoint(
                    name=name,
                    side=side,
                    jtype=jtype,
                    num=num
                )

            setattr(self._matched_joints[key], field_name, pos)

        # Emit the current consolidated state
        sorted_joints = sorted(
            self._matched_joints.values(),
            key=lambda j: (j.side, j.jtype, j.num, j.name)
        )

        display_names = [j.name for j in sorted_joints]
        cmd_vals      = [j.cmd for j in sorted_joints]
        actual_vals   = [j.actual for j in sorted_joints]

        self._on_data(
            display_names,
            cmd_vals,
            actual_vals,
            self._cmd_live,
            self._actual_live
        )

    @staticmethod
    def _normalize_name(name: str, default_side: Optional[int] = None) -> Tuple[int, str]:
        """
        Normalize a joint name into a side indicator and a core suffix.
        Side: 0 = left, 1 = right, 2 = other
        Suffix: joint suffix (e.g. 'joint1', 'finger_joint1')
        """
        name_lower = name.lower()
        if "left" in name_lower or name_lower.startswith("l_"):
            side = 0
        elif "right" in name_lower or name_lower.startswith("r_"):
            side = 1
        elif default_side is not None:
            side = default_side
        else:
            side = 2

        suffix = name_lower
        for prefix in ["daksha_left_", "daksha_right_", "leader_left_", "leader_right_", "leader/left_", "leader/right_", "left_", "right_", "daksha_"]:
            if suffix.startswith(prefix):
                suffix = suffix[len(prefix):]
                break
        return side, suffix


class RosInterface:
    """
    Public API for managing the ROS 2 connection.
    """

    def __init__(self) -> None:
        if not rclpy.ok():
            rclpy.init()
        self._node:        Optional[_AnalyzerNode]        = None
        self._executor:    Optional[MultiThreadedExecutor] = None
        self._spin_thread: Optional[threading.Thread]     = None

    # ── Connection ─────────────────────────────────────────────────────────────

    def connect(
        self,
        follower_cmd_topic:   str,
        follower_state_topic:  str,
        on_data:              Callable[[List[str], List[Optional[float]], List[Optional[float]], bool, bool], None],
        on_error:             Callable[[str], None],
    ) -> None:
        self.disconnect()

        self._node = _AnalyzerNode(
            follower_cmd_topic   = follower_cmd_topic,
            follower_state_topic = follower_state_topic,
            on_data              = on_data,
            on_error             = on_error,
        )

        self._executor = MultiThreadedExecutor()
        self._executor.add_node(self._node)

        self._spin_thread = threading.Thread(
            target=self._executor.spin,
            daemon=True,
            name="ros2_executor",
        )
        self._spin_thread.start()

    def disconnect(self) -> None:
        if self._executor is not None:
            self._executor.shutdown(timeout_sec=1.0)
            self._executor = None
        if self._node is not None:
            self._node.destroy_node()
            self._node = None
        if self._spin_thread is not None and self._spin_thread.is_alive():
            self._spin_thread.join(timeout=2.0)
            self._spin_thread = None

    # ── Topic Discovery ────────────────────────────────────────────────────────

    @staticmethod
    def get_joint_state_topics() -> List[str]:
        """Return all currently advertised JointState topic names."""
        topics: List[str] = []
        try:
            if not rclpy.ok():
                rclpy.init()
            tmp = rclpy.create_node("_joint_analyzer_discovery")
            topic_list = tmp.get_topic_names_and_types()
            tmp.destroy_node()
            for name, types in topic_list:
                for t in types:
                    if "JointState" in t:
                        topics.append(name)
                        break
        except Exception as exc:  # noqa: BLE001
            print(f"[RosInterface] Topic discovery failed: {exc}")
        return sorted(topics)

    @staticmethod
    def check_topics_publishing(topic_names: List[str]) -> Dict[str, bool]:
        """
        Check whether topics have active publishers.
        """
        result: Dict[str, bool] = {name: False for name in topic_names}
        try:
            if not rclpy.ok():
                rclpy.init()
            tmp = rclpy.create_node("_joint_analyzer_pub_check")
            for name in topic_names:
                try:
                    pub_info = tmp.get_publishers_info_by_topic(name)
                    result[name] = len(pub_info) > 0
                except Exception:
                    result[name] = False
            tmp.destroy_node()
        except Exception as exc:  # noqa: BLE001
            print(f"[RosInterface] Publisher check failed: {exc}")
        return result

    @staticmethod
    def set_domain_id(domain_id: int) -> None:
        """
        Change the ROS_DOMAIN_ID and restart rclpy.
        """
        if rclpy.ok():
            try:
                rclpy.shutdown()
            except Exception:
                pass

        os.environ["ROS_DOMAIN_ID"] = str(domain_id)

        try:
            rclpy.init()
        except Exception as exc:
            print(f"[RosInterface] rclpy re-init failed: {exc}")

    def shutdown(self) -> None:
        self.disconnect()
        # rclpy installs its own SIGINT handler that may already have shut
        # the context down by the time we get here (Ctrl-C races this
        # against that handler) — that's expected, not an error, so
        # swallow it rather than let a benign double-shutdown print a
        # traceback on every normal Ctrl-C.
        try:
            if rclpy.ok():
                rclpy.shutdown()
        except Exception:
            pass
