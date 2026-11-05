#!/usr/bin/env python3
"""Joint target for position control (hw_interface 'posvel' = DM POS_VEL mode), one joint.

Position-mode counterpart of joint_cmd.py (which is for MIT mode):
  - /joint_cmd (sensor_msgs/JointState) sets the target position,
  - at publish_rate one JointTrajectory point (target, speed limit) is sent to a
    posveleff_controllers/PosVelEffController, which limits the target's velocity and
    acceleration and hands the profile position to hw_interface.

In POS_VEL the motor's own position loop tracks the position, and the velocity field is its
SPEED LIMIT for the move, not a feedforward. So the controller runs with
send_profile_velocity: false and forwards this constant positive speed_limit instead of the
profile velocity (0 at rest, which could leave the motor short of the target). There is
no torque input in POS_VEL, so no feedforward and effort is 0.

hw_interface multiplies the velocity by the joint's 'direction', so 'direction' here must equal
that hw parameter; the speed limit is pre-multiplied by it to reach the motor positive.
"""

import math

import rclpy
from rcl_interfaces.msg import SetParametersResult
from rclpy.node import Node
from rclpy.qos import (
    QoSDurabilityPolicy,
    QoSHistoryPolicy,
    QoSProfile,
    QoSReliabilityPolicy,
)

from builtin_interfaces.msg import Duration
from sensor_msgs.msg import JointState
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint


class PositionCommand(Node):

    def __init__(self):
        super().__init__("position_command")

        # -------- Parameters --------
        self.declare_parameter("joint_name", "joint1")
        self.declare_parameter("controller_topic",
                               "/joint1_position_controller/joint_trajectory")
        self.declare_parameter("publish_rate", 200.0)     # Hz
        # POS_VEL speed limit sent to the motor [rad/s]. Keep it above the controller's
        # max_velocity so the motor keeps up with the profile.
        self.declare_parameter("speed_limit", 2.0)
        self.declare_parameter("direction", -1.0)         # must equal hw_interface 'direction'

        self.joint_name = self.get_parameter("joint_name").value
        self.rate = self.get_parameter("publish_rate").value
        self.speed_limit = self.get_parameter("speed_limit").value
        self.direction = self.get_parameter("direction").value
        self.dt = 1.0 / self.rate

        if self.direction not in (1.0, -1.0):
            raise ValueError("direction must be 1.0 or -1.0")
        if not self.speed_limit > 0.0:
            raise ValueError("speed_limit must be > 0 (0 stops the motor in POS_VEL)")

        self.add_on_set_parameters_callback(self.dynamic_parameter_cb)

        # -------- State --------
        self.target_pos = 0.0
        self.initialized = False

        # -------- ROS --------
        self.create_subscription(
            JointState, "/joint_states", self.joint_state_callback, 10)

        # Live command stream: only the newest command matters.
        self.create_subscription(
            JointState,
            "/joint_cmd",
            self.cmd_callback,
            QoSProfile(
                reliability=QoSReliabilityPolicy.BEST_EFFORT,
                durability=QoSDurabilityPolicy.VOLATILE,
                history=QoSHistoryPolicy.KEEP_LAST,
                depth=1,
            ))

        self.pub = self.create_publisher(
            JointTrajectory, self.get_parameter("controller_topic").value, 10)

        self.timer = self.create_timer(self.dt, self.publish)

        self.get_logger().info(
            f"Commanding {self.joint_name} on {self.get_parameter('controller_topic').value} "
            f"(POS_VEL, speed limit {self.speed_limit} rad/s)")

    def dynamic_parameter_cb(self, params):

        for p in params:

            if p.name == "speed_limit":
                if not p.value > 0.0:
                    return SetParametersResult(
                        successful=False, reason="speed_limit must be > 0")
                self.speed_limit = p.value
            elif p.name in ("joint_name", "controller_topic", "publish_rate", "direction"):
                return SetParametersResult(
                    successful=False, reason=f"{p.name} is read-only after start")

            self.get_logger().info(f"[param] {p.name} -> {p.value}")

        return SetParametersResult(successful=True)

    def joint_state_callback(self, msg):

        # Until the first /joint_cmd, hold where the joint actually is.
        if self.initialized or self.joint_name not in msg.name:
            return
        i = msg.name.index(self.joint_name)
        if i >= len(msg.position) or not math.isfinite(msg.position[i]):
            return

        self.target_pos = msg.position[i]
        self.initialized = True

    def cmd_callback(self, msg):

        if self.joint_name not in msg.name:
            return
        i = msg.name.index(self.joint_name)

        if i < len(msg.position) and math.isfinite(msg.position[i]):
            self.target_pos = msg.position[i]

    def publish(self):

        if not self.initialized:
            return

        traj = JointTrajectory()
        traj.joint_names = [self.joint_name]
        p = JointTrajectoryPoint()
        p.positions = [self.target_pos]
        # hw_interface multiplies velocity by 'direction'; pre-multiply so the motor gets a
        # positive speed limit.
        p.velocities = [self.direction * self.speed_limit]
        p.effort = [0.0]
        p.time_from_start = Duration(sec=0, nanosec=int(self.dt * 1e9))
        traj.points.append(p)
        self.pub.publish(traj)


def main(args=None):

    rclpy.init(args=args)
    node = PositionCommand()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == "__main__":
    main()
