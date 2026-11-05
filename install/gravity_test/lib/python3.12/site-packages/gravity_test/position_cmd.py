#!/usr/bin/env python3
"""Joint command limiter for position control (hw_interface 'posvel' = DM POS_VEL mode).

Position-mode counterpart of joint_cmd.py (which is for MIT mode), for one joint:
  - /joint_cmd (sensor_msgs/JointState) sets the target position,
  - the target is velocity/acceleration limited at publish_rate,
  - the result is sent as one JointTrajectory point to a
    posveleff_controllers/PosVelEffController, which hands it to hw_interface.

In POS_VEL the motor's own position loop tracks the position, and the velocity field is its
SPEED LIMIT for the move, not a feedforward. So instead of the ramp velocity (0 at rest, which
could leave the motor short of the target) a constant positive speed_limit is sent. There is
no torque input in POS_VEL, so no gravity feedforward and effort is 0.

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


class PositionCommandLimiter(Node):

    def __init__(self):
        super().__init__("position_command_limiter")

        # -------- Parameters --------
        self.declare_parameter("joint_name", "joint1")
        self.declare_parameter("controller_topic",
                               "/joint1_position_controller/joint_trajectory")
        self.declare_parameter("max_velocity", 1.0)       # target ramp speed [rad/s]
        self.declare_parameter("max_acceleration", 2.0)   # target ramp accel [rad/s^2]
        self.declare_parameter("publish_rate", 200.0)     # Hz
        # POS_VEL speed limit sent to the motor [rad/s]. Keep it above max_velocity so the
        # motor keeps up with the ramp.
        self.declare_parameter("speed_limit", 2.0)
        self.declare_parameter("direction", -1.0)         # must equal hw_interface 'direction'

        self.joint_name = self.get_parameter("joint_name").value
        self.max_vel = self.get_parameter("max_velocity").value
        self.max_acc = self.get_parameter("max_acceleration").value
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
        self.current_pos = 0.0
        self.current_vel = 0.0
        self.initialized = False
        self._last_update_time = None

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

        # URDF-convention debug view of the limited target (same topic as joint_cmd.py, so
        # joint_analyzer compares /jnt_cmt_to_ctrl against /joint_states the same way).
        self.debug_pub = self.create_publisher(JointState, "/jnt_cmt_to_ctrl", 10)

        self.timer = self.create_timer(self.dt, self.update)

        self.get_logger().info(
            f"Commanding {self.joint_name} on {self.get_parameter('controller_topic').value} "
            f"(POS_VEL, speed limit {self.speed_limit} rad/s)")

    def dynamic_parameter_cb(self, params):

        for p in params:

            if p.name == "max_velocity":
                self.max_vel = p.value
            elif p.name == "max_acceleration":
                self.max_acc = p.value
            elif p.name == "speed_limit":
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

        # Start from where the joint actually is.
        if self.initialized or self.joint_name not in msg.name:
            return
        i = msg.name.index(self.joint_name)
        if i >= len(msg.position) or not math.isfinite(msg.position[i]):
            return

        self.current_pos = msg.position[i]
        self.target_pos = msg.position[i]
        self.initialized = True

    def cmd_callback(self, msg):

        if self.joint_name not in msg.name:
            return
        i = msg.name.index(self.joint_name)

        if i < len(msg.position) and math.isfinite(msg.position[i]):
            self.target_pos = msg.position[i]

    def update(self):

        if not self.initialized:
            return

        # Integrate over the real elapsed time, not an assumed self.dt.
        now = self.get_clock().now()
        if self._last_update_time is None:
            self._last_update_time = now
            return
        dt = (now - self._last_update_time).nanoseconds * 1e-9
        self._last_update_time = now
        if dt <= 0.0:
            return
        dt = min(dt, 2.0 * self.dt)

        error = self.target_pos - self.current_pos

        # Cap speed so the target can still brake to zero at the goal (v^2 = 2*a*d).
        desired_vel = math.copysign(
            min(self.max_vel, math.sqrt(2 * self.max_acc * abs(error))), error)

        max_dv = self.max_acc * dt
        self.current_vel += max(-max_dv, min(max_dv, desired_vel - self.current_vel))

        # Snap onto the target once it is reachable within this step.
        if abs(error) <= abs(self.current_vel) * dt and abs(self.current_vel) <= max_dv:
            self.current_pos = self.target_pos
            self.current_vel = 0.0
        else:
            self.current_pos += self.current_vel * dt

        self.publish(now)

    def publish(self, now):

        traj = JointTrajectory()
        traj.joint_names = [self.joint_name]
        p = JointTrajectoryPoint()
        p.positions = [self.current_pos]
        # hw_interface multiplies velocity by 'direction'; pre-multiply so the motor gets a
        # positive speed limit.
        p.velocities = [self.direction * self.speed_limit]
        p.effort = [0.0]
        p.time_from_start = Duration(sec=0, nanosec=int(self.dt * 1e9))
        traj.points.append(p)
        self.pub.publish(traj)

        debug = JointState()
        debug.header.stamp = now.to_msg()
        debug.name = [self.joint_name]
        debug.position = [self.current_pos]
        debug.velocity = [self.current_vel]
        debug.effort = [0.0]
        self.debug_pub.publish(debug)


def main(args=None):

    rclpy.init(args=args)
    node = PositionCommandLimiter()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == "__main__":
    main()
