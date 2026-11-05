#!/usr/bin/env python3
import os
import time

# Force CycloneDDS to send user data via unicast rather than multicast.
# This prevents data streams (like /joint_cmd) from being blocked by the network switch.
os.environ["CYCLONEDDS_URI"] = (
    "<CycloneDDS><Domain><General>"
    "<AllowMulticast>spdp</AllowMulticast>"
    "</General></Domain></CycloneDDS>"
)

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from rclpy.qos import qos_profile_sensor_data

class QoSDiagnostic(Node):
    def __init__(self):
        super().__init__('qos_diagnostic')
        
        print("\n=== ROS 2 Network & Topic Diagnostic ===")
        
        # Print active topics
        try:
            topics = self.get_topic_names_and_types()
            print("\nAvailable Topics in the ROS 2 Graph:")
            for name, types in topics:
                print(f"  - {name} ({', '.join(types)})")
        except Exception as e:
            print(f"Error listing topics: {e}")

        # Check publishers for /joint_cmd
        try:
            pubs = self.get_publishers_info_by_topic('/joint_cmd')
            print(f"\nPublishers for /joint_cmd: {len(pubs)}")
            for i, p in enumerate(pubs):
                print(f"  Publisher {i+1}:")
                print(f"    Node: {p.node_namespace}/{p.node_name}")
                print(f"    Reliability: {p.qos_profile.reliability}")
                print(f"    Durability: {p.qos_profile.durability}")
        except Exception as e:
            print(f"Error checking /joint_cmd publishers: {e}")

        # Check publishers for LeftArmSystem_ordered_joint_states
        try:
            pubs = self.get_publishers_info_by_topic('/LeftArmSystem_ordered_joint_states')
            print(f"\nPublishers for /LeftArmSystem_ordered_joint_states: {len(pubs)}")
            for i, p in enumerate(pubs):
                print(f"  Publisher {i+1}:")
                print(f"    Node: {p.node_namespace}/{p.node_name}")
                print(f"    Reliability: {p.qos_profile.reliability}")
                print(f"    Durability: {p.qos_profile.durability}")
        except Exception as e:
            print(f"Error checking /LeftArmSystem_ordered_joint_states publishers: {e}")

        # Subscribe to both to test live data transmission
        self.received_cmd = 0
        self.received_state = 0
        
        self.sub_cmd = self.create_subscription(
            JointState, '/joint_cmd', 
            self.cmd_cb, 
            qos_profile_sensor_data
        )
        
        self.sub_state = self.create_subscription(
            JointState, '/LeftArmSystem_ordered_joint_states', 
            self.state_cb, 
            qos_profile_sensor_data
        )

        print("\n[Diagnostic] Subscribed and listening for 10 seconds. PLEASE MAKE SURE THE ROBOT IS REPLAYING/MOVING NOW...")

    def cmd_cb(self, msg):
        self.received_cmd += 1
        if self.received_cmd == 1 or self.received_cmd % 10 == 0:
            print(f"  -> [DATA RECEIVED] /joint_cmd (count: {self.received_cmd})")

    def state_cb(self, msg):
        self.received_state += 1
        if self.received_state == 1 or self.received_state % 10 == 0:
            print(f"  -> [DATA RECEIVED] /LeftArmSystem_ordered_joint_states (count: {self.received_state})")

def main():
    rclpy.init()
    node = QoSDiagnostic()
    start = time.time()
    try:
        while time.time() - start < 10.0:
            rclpy.spin_once(node, timeout_sec=0.1)
    except KeyboardInterrupt:
        pass
    
    print("\n=== Test Results ===")
    print(f"Total messages received on /joint_cmd:                       {node.received_cmd}")
    print(f"Total messages received on /LeftArmSystem_ordered_joint_states: {node.received_state}")
    
    if node.received_state > 0 and node.received_cmd == 0:
        print("\n[ROOT CAUSE IDENTIFIED]")
        print("Your laptop (Thunder) is successfully receiving actual state data from the robot,")
        print("but the command data (/joint_cmd) is not arriving over the network.")
        print("This means the replay node on the robot (s1) is either not publishing during this test,")
        print("or its messages are blocked by firewall/DDS routing policies on the robot PC.")
    elif node.received_state == 0 and node.received_cmd == 0:
        print("\n[NO DATA RECEIVED]")
        print("Neither topic received any messages. Ensure the robot is replaying/moving and Domain ID is correct.")
    else:
        print("\n[SUCCESS] Both topics are communicating correctly!")
        
    print("\nDiagnostic complete.\n")
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
