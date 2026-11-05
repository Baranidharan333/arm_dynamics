#!/usr/bin/env python3
"""
verify_mapping.py
-----------------
Diagnostic script to double-check that joint matching and differences are
100% accurate between Follower Command (/joint_cmd) and Follower Actual (/joint_states).

Run with:
    export ROS_DOMAIN_ID=0
    source /opt/ros/jazzy/setup.bash
    python3 verify_mapping.py
"""

import os
import re
import sys
import time
import math
from typing import Dict, Tuple, Optional

# Force CycloneDDS to send user data via unicast rather than multicast.
# This prevents data streams (like /joint_cmd) from being blocked by the network switch.
os.environ["CYCLONEDDS_URI"] = (
    "<CycloneDDS><Domain><General>"
    "<AllowMulticast>spdp</AllowMulticast>"
    "</General></Domain></CycloneDDS>"
)

import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import JointState


def normalize_name(name: str) -> Tuple[int, str]:
    name_lower = name.lower()
    if "left" in name_lower or name_lower.startswith("l_"):
        side = 0
    elif "right" in name_lower or name_lower.startswith("r_"):
        side = 1
    else:
        side = 2

    suffix = name_lower
    for prefix in ["daksha_left_", "daksha_right_", "leader_left_", "leader_right_", "leader/left_", "leader/right_", "left_", "right_", "daksha_"]:
        if suffix.startswith(prefix):
            suffix = suffix[len(prefix):]
            break
    return side, suffix


class MappingVerifier(Node):

    def __init__(self):
        super().__init__("mapping_verifier")

        self.follower_cmd_msg:   Optional[JointState] = None
        self.follower_state_msg: Optional[JointState] = None

        self.create_subscription(JointState, "/joint_cmd",    self.follower_cmd_cb,  qos_profile_sensor_data)
        self.create_subscription(JointState, "/joint_states", self.follower_state_cb, qos_profile_sensor_data)

        print("[Verifier] Subscribed to:")
        print("  - /joint_cmd")
        print("  - /joint_states")
        print("[Verifier] Waiting for messages on both active topics...")

    def follower_cmd_cb(self, msg):
        self.follower_cmd_msg = msg

    def follower_state_cb(self, msg):
        self.follower_state_msg = msg

    def print_diagnostic(self):
        # Gather all unique matched joints
        matched = {}  # (side, suffix) -> { "cmd": val, "actual": val, "cmd_name": str, "actual_name": str }

        def process_msg(msg, field):
            if msg is None:
                return
            for name, pos in zip(msg.name, msg.position):
                side, suffix = normalize_name(name)
                key = (side, suffix)
                if key not in matched:
                    matched[key] = {
                        "cmd": None, "actual": None,
                        "cmd_name": "", "actual_name": ""
                    }
                matched[key][field] = pos
                matched[key][f"{field}_name"] = name

        process_msg(self.follower_cmd_msg, "cmd")
        process_msg(self.follower_state_msg, "actual")

        if not matched:
            print("[Verifier] No joint data received yet.")
            return

        # Sort keys logically: Left regular, Left finger, Right regular, Right finger
        def sort_key(k):
            side, suffix = k
            jtype = 1 if any(x in suffix for x in ["finger", "gripper", "thumb"]) else 0
            match = re.search(r'\d+', suffix)
            num = int(match.group()) if match else 0
            return (side, jtype, num, suffix)

        sorted_keys = sorted(matched.keys(), key=sort_key)

        print("\n" + "="*100)
        print("                              JOINT MAPPING & DIFFERENCE DIAGNOSTIC")
        print("="*100)
        print(f"{'Joint Name (Cleaned)':<24} | {'Follower Target (Cmd)':<22} | {'Follower Actual':<22} | {'Difference (deg)':<18}")
        print("-"*100)

        for k in sorted_keys:
            side_str = "left" if k[0] == 0 else "right" if k[0] == 1 else "other"

            data = matched[k]
            c_val = data["cmd"]
            a_val = data["actual"]

            c_name = data["cmd_name"]
            a_name = data["actual_name"]

            # Show the real name as published on the bus, not a fabricated
            # one - this robot's joints are plain "left_joint_1" etc, no
            # "daksha_" prefix, so inventing one here would mislabel the row.
            display_name = c_name or a_name or f"{side_str}_{k[1]}"

            # Format values to degrees
            c_deg = f"{math.degrees(c_val):.4f}°" if c_val is not None else "—"
            a_deg = f"{math.degrees(a_val):.4f}°" if a_val is not None else "—"
            diff = f"{math.degrees(abs(c_val - a_val)):.6f}°" if c_val is not None and a_val is not None else "—"

            print(f"{display_name:<24} | {c_deg:<22} | {a_deg:<22} | {diff:<18}")
            # Sub-row for topic name matching verification
            print(f"{'':<24} | {c_name[:22]:<22} | {a_name[:22]:<22} | {'':<18}")
            print("-"*100)

        print("\n[Status Report]")
        print(f"  - /joint_cmd:    {'OK (received)' if self.follower_cmd_msg else 'Waiting...'}")
        print(f"  - /joint_states: {'OK (received)' if self.follower_state_msg else 'Waiting...'}")
        print("="*100 + "\n")


def main():
    rclpy.init()
    node = MappingVerifier()
    try:
        while rclpy.ok():
            rclpy.spin_once(node, timeout_sec=1.0)
            node.print_diagnostic()
            time.sleep(2.0)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
