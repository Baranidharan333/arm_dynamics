#!/usr/bin/env python3
"""Joint command limiter with gravity feedforward for the single-joint rig.

Same job as gen2's dual-arm joint_cmd.py, for one joint:
  - /joint_cmd (sensor_msgs/JointState) sets the target position and optional effort,
  - the target is velocity/acceleration limited at publish_rate,
  - the result is sent as one JointTrajectory point (p, v, effort) to a
    posveleff_controllers/PosVelEffController, which hands it to hw_interface.

The effort sent is
    effort_sign * (target_effort + gravity_torque)
where gravity_torque comes from gravity_compensation's gravity_torque_node on /gravity_torque
(URDF convention, computed from /robot_description at the measured position).
effort_sign must equal the joint's hw_interface 'direction': ArmHardwareInterface flips
position/velocity by it but sends effort unflipped.
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


class JointCommandLimiter(Node):

    def __init__(self):
        super().__init__("joint_command_limiter")

        # -------- Parameters --------
        self.declare_parameter("joint_name", "joint1")
        self.declare_parameter("controller_topic",
                               "/single_joint_mit_controller/joint_trajectory")
        self.declare_parameter("max_velocity", 1.0)       # rad/s
        self.declare_parameter("max_acceleration", 2.0)   # rad/s^2
        self.declare_parameter("publish_rate", 200.0)     # Hz

        # Gravity feedforward from /gravity_torque
        self.declare_parameter("gravity_enabled", True)
        # Older than this, the gravity torque is dropped: it was computed for another pose.
        self.declare_parameter("gravity_timeout", 0.1)    # s
        self.declare_parameter("effort_sign", -1.0)
        self.declare_parameter("max_effort", 4.0)         # |effort| clamp [Nm]

        self.joint_name = self.get_parameter("joint_name").value
        self.max_vel = self.get_parameter("max_velocity").value
        self.max_acc = self.get_parameter("max_acceleration").value
        self.rate = self.get_parameter("publish_rate").value
        self.gravity_enabled = self.get_parameter("gravity_enabled").value
        self.gravity_timeout = self.get_parameter("gravity_timeout").value
        self.effort_sign = self.get_parameter("effort_sign").value
        self.max_effort = self.get_parameter("max_effort").value
        self.dt = 1.0 / self.rate

        if self.effort_sign not in (1.0, -1.0):
            raise ValueError("effort_sign must be 1.0 or -1.0")

        self.add_on_set_parameters_callback(self.dynamic_parameter_cb)

        # -------- State --------
        self.target_pos = 0.0
        self.target_effort = 0.0
        self.current_pos = 0.0
        self.current_vel = 0.0
        self.gravity_effort = 0.0
        self._gravity_stamp = None
        self._gravity_stale = True
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

        self.create_subscription(
            JointState, "/gravity_torque", self.gravity_effort_callback, 10)

        self.pub = self.create_publisher(
            JointTrajectory, self.get_parameter("controller_topic").value, 10)

        # URDF-convention debug view: limited target, and the gravity torque used in effort.
        self.debug_pub = self.create_publisher(JointState, "/jnt_cmt_to_ctrl", 10)

        self.timer = self.create_timer(self.dt, self.update)

        self.get_logger().info(
            f"Commanding {self.joint_name} on {self.get_parameter('controller_topic').value}, "
            f"gravity from /gravity_torque {'on' if self.gravity_enabled else 'off'}")

    def dynamic_parameter_cb(self, params):

        for p in params:

            if p.name == "max_velocity":
                self.max_vel = p.value
            elif p.name == "max_acceleration":
                self.max_acc = p.value
            elif p.name == "gravity_enabled":
                self.gravity_enabled = p.value
            elif p.name == "gravity_timeout":
                self.gravity_timeout = p.value
            elif p.name == "max_effort":
                self.max_effort = p.value
            elif p.name in ("joint_name", "controller_topic", "publish_rate", "effort_sign"):
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

        if i < len(msg.position):
            self.target_pos = msg.position[i]
        if i < len(msg.effort):
            self.target_effort = msg.effort[i]

    def gravity_effort_callback(self, msg):

        if self.joint_name not in msg.name:
            return
        i = msg.name.index(self.joint_name)
        if i >= len(msg.effort) or not math.isfinite(msg.effort[i]):
            return

        self.gravity_effort = msg.effort[i]
        self._gravity_stamp = self.get_clock().now()

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

        # Cap speed so the joint can still brake to zero at the target (v^2 = 2*a*d).
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

    def gravity(self, now):

        if not self.gravity_enabled:
            return 0.0

        stale = self._gravity_stamp is None or \
            (now - self._gravity_stamp).nanoseconds * 1e-9 > self.gravity_timeout
        if stale != self._gravity_stale:
            self._gravity_stale = stale
            if stale:
                self.get_logger().warn(
                    f"No /gravity_torque for {self.joint_name} in {self.gravity_timeout}s; "
                    "sending no gravity torque")
            else:
                self.get_logger().info("Receiving /gravity_torque")
        return 0.0 if stale else self.gravity_effort

    def publish(self, now):

        gravity = self.gravity(now)
        effort = max(-self.max_effort, min(self.max_effort, self.target_effort + gravity))

        traj = JointTrajectory()
        traj.joint_names = [self.joint_name]
        p = JointTrajectoryPoint()
        p.positions = [self.current_pos]
        p.velocities = [self.current_vel]
        p.effort = [self.effort_sign * effort]
        p.time_from_start = Duration(sec=0, nanosec=int(self.dt * 1e9))
        traj.points.append(p)
        self.pub.publish(traj)

        debug = JointState()
        debug.header.stamp = now.to_msg()
        debug.name = [self.joint_name]
        debug.position = [self.current_pos]
        debug.velocity = [self.current_vel]
        debug.effort = [gravity]
        self.debug_pub.publish(debug)


def main(args=None):

    rclpy.init(args=args)
    node = JointCommandLimiter()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == "__main__":
    main()
