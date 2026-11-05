#!/usr/bin/env python3
import sys
import os
import time
import math

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

class ROS2TopicChecker(Node):
    def __init__(self):
        super().__init__('ros2_topic_checker')
        self.cmd_msg = None
        self.state_msg = None

        self.sub_cmd = self.create_subscription(
            JointState, '/joint_cmd', self.cmd_cb, qos_profile_sensor_data
        )
        self.sub_state = self.create_subscription(
            JointState, '/joint_states', self.state_cb, qos_profile_sensor_data
        )

        print(f"[Checker] Active ROS_DOMAIN_ID: {os.environ.get('ROS_DOMAIN_ID', '0 (Default)')}")
        print("[Checker] Subscribed to:")
        print("  - /joint_cmd")
        print("  - /joint_states")
        print("[Checker] Waiting for messages...")

    def cmd_cb(self, msg):
        self.cmd_msg = msg

    def state_cb(self, msg):
        self.state_msg = msg

    def has_all_data(self):
        return self.cmd_msg is not None and self.state_msg is not None

    def print_comparison(self):
        print("\n" + "="*80)
        print("                   JOINT COMPARISON & DIFFERENCE DIAGNOSTIC")
        print("="*80)
        print(f"{'Joint Name':<30} | {'Command (deg)':<15} | {'Actual (deg)':<15} | {'Difference (deg)':<15}")
        print("-"*80)

        cmd_dict = dict(zip(self.cmd_msg.name, self.cmd_msg.position))
        state_dict = dict(zip(self.state_msg.name, self.state_msg.position))

        all_names = sorted(list(set(cmd_dict.keys()) | set(state_dict.keys())))

        for name in all_names:
            c_val = cmd_dict.get(name, None)
            a_val = state_dict.get(name, None)

            c_str = f"{math.degrees(c_val):.4f}°" if c_val is not None else "—"
            a_str = f"{math.degrees(a_val):.4f}°" if a_val is not None else "—"
            diff_str = f"{math.degrees(abs(c_val - a_val)):.6f}°" if c_val is not None and a_val is not None else "—"

            print(f"{name:<30} | {c_str:<15} | {a_str:<15} | {diff_str:<15}")
        print("="*80 + "\n")

def main():
    rclpy.init()
    node = ROS2TopicChecker()
    
    start_time = time.time()
    try:
        while rclpy.ok():
            rclpy.spin_once(node, timeout_sec=0.1)
            if node.has_all_data():
                node.print_comparison()
                break
            
            # Print warning every 5 seconds if command is missing
            if time.time() - start_time > 5.0:
                missing = []
                if node.cmd_msg is None: missing.append("/joint_cmd (command)")
                if node.state_msg is None: missing.append("/joint_states (actual)")
                print(f"[Warning] Still waiting for: {', '.join(missing)}")
                start_time = time.time()
                
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
