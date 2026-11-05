#!/usr/bin/env python3
"""Home / go pose services for the dual arm.

Services
  /move_home          std_srvs/Trigger              move to the home pose (0 rad per joint)
  /move_go            std_srvs/Trigger              move to the current go pose
  /set_go_position    gen2_msgs/SetHomePosition     set the go pose, does not move
                                                    (empty position = current measured pose)
  /reset_go_position  std_srvs/Trigger              go pose back to 0 rad per joint

Nothing about the joints is hardcoded: the joint list (names, count, order) is read from the
measured joint states on joint_state_topics (default /LeftArmSystem_ordered_joint_states then
/RightArmSystem_ordered_joint_states; /DualJointSystem_ordered_joint_states on the two-joint
rig), every time a pose is used.
  - home = 0 rad for every measured joint, except home_overrides,
  - go   = 0 rad for every measured joint until /set_go_position is called; then the values
           set there, stored per joint name.
/set_go_position takes one value per measured joint in that joint-state order, or none to
capture the current pose. Every move is built by joint name in the measured order, so a change
in order cannot send a value to the wrong joint, and a pose that does not cover exactly the
measured joints is refused.

A move starts from the latest measured position, follows a smoothstep to the target over
move_duration seconds on /joint_cmd, then waits up to settle_timeout for the measured position
to be within goal_tolerance. The Trigger response comes back when the move has finished, so
callers like 'ros2 service call /move_home ...' block until then (leader_controller_ui.py,
dashboard_app.py and bring_up.py rely on that). Moves run in their own thread: joint states and
the set/reset services keep working during a move, and a second move request is refused.

Parameters
  joint_state_topics   the two gen2 ordered joint-state topics (see JOINT_STATE_TOPICS)
  home_overrides       []    home values for single joints, "joint_name:value" [rad]
  move_duration        5.0   s
  publish_rate         30.0  Hz
  state_timeout        0.5   s, refuse to move on older joint states
  goal_tolerance       0.05  rad, max |measured - target| to report success
  settle_timeout       2.0   s, how long to wait for the measured position after the path

The PosVelEffControllers still apply their own max_velocity / max_acceleration on top.
"""

import math
import threading
import time

import numpy as np
import rclpy
from gen2_msgs.srv import SetHomePosition
from rclpy.callback_groups import MutuallyExclusiveCallbackGroup, ReentrantCallbackGroup
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_srvs.srv import Trigger

# Ordered joint-state topics (hw_interface publishes /<ros2_control system name>_ordered_
# joint_states). Their joint lists are joined in this order. Default: the gen2 dual arm.
# Test rigs: ["/DualJointSystem_ordered_joint_states"] or
#            ["/SingleJointSystem_ordered_joint_states"].
JOINT_STATE_TOPICS = [
    "/LeftArmSystem_ordered_joint_states",
    "/RightArmSystem_ordered_joint_states",
]

# Poses are built from the measured joint states at the time of use: one value per joint the
# ordered joint-state topics carry, in their order, so the joint count and names are never
# hardcoded here.
#
# Home = HOME_VALUE for every measured joint, except joints listed in the home_overrides
# parameter ("joint_name:value" strings).
# NOTE: on the gen2 right arm, 0.0 is the lower mechanical stop of right_joint_4
# (0..2.094 rad) and right_joint_5 (0..3.142 rad), so an all-zero home drives them into their
# stops. The previously tuned right-arm mid-range pose, as overrides:
#   ["right_joint_1:-0.34", "right_joint_4:1.45", "right_joint_5:1.53", "right_joint_7:0.7"]
HOME_VALUE = 0.0

# Go pose until /set_go_position is called, and after /reset_go_position.
DEFAULT_GO_VALUE = 0.0


class HomeMoveService(Node):

    def __init__(self):
        super().__init__("home_move_service")

        self.declare_parameter("joint_state_topics", JOINT_STATE_TOPICS)
        self.declare_parameter("home_overrides", [""])
        self.declare_parameter("move_duration", 5.0)
        self.declare_parameter("publish_rate", 30.0)
        self.declare_parameter("state_timeout", 0.5)
        self.declare_parameter("goal_tolerance", 0.05)
        self.declare_parameter("settle_timeout", 2.0)

        self.home_overrides = self._parse_overrides(self.get_parameter("home_overrides").value)

        # Go pose per joint name, or None = DEFAULT_GO_VALUE for every measured joint.
        self.go_position = None

        # Latest measured state per topic and when it arrived, in joint_state_topics order.
        self.state_topics = [t for t in self.get_parameter("joint_state_topics").value if t]
        if not self.state_topics:
            raise ValueError("joint_state_topics must list at least one topic")
        self._state_lock = threading.Lock()
        self.states = [None] * len(self.state_topics)
        self.stamps = [None] * len(self.state_topics)

        # One move at a time.
        self._move_lock = threading.Lock()

        # States and set/reset run alongside a move; moves run in their own group.
        io_group = MutuallyExclusiveCallbackGroup()
        move_group = ReentrantCallbackGroup()

        for i, topic in enumerate(self.state_topics):
            self.create_subscription(
                JointState, topic, lambda msg, i=i: self._state_cb(i, msg), 10,
                callback_group=io_group)

        self.pub = self.create_publisher(JointState, "/joint_cmd", 10)

        self.create_service(Trigger, "/move_home", self.move_home_cb, callback_group=move_group)
        self.create_service(Trigger, "/move_go", self.move_go_cb, callback_group=move_group)
        self.create_service(
            SetHomePosition, "/set_go_position", self.set_go_position_cb,
            callback_group=io_group)
        self.create_service(
            Trigger, "/reset_go_position", self.reset_go_position_cb, callback_group=io_group)

        self.get_logger().info(f"Joint states from {', '.join(self.state_topics)}")
        self.get_logger().info(
            f"Home: {HOME_VALUE} rad for every measured joint"
            + (f", overrides {self._fmt(self.home_overrides)}" if self.home_overrides else ""))

    # ------------------------------------------------------------------ helpers

    @staticmethod
    def _parse_overrides(entries):
        """["joint_name:value", ...] -> {joint_name: value}. Empty strings are ignored."""
        overrides = {}
        for entry in entries:
            if not entry.strip():
                continue
            name, sep, value = entry.partition(":")
            try:
                v = float(value)
            except ValueError:
                v = math.nan
            if not sep or not name.strip() or not math.isfinite(v):
                raise ValueError(f"home_overrides entry '{entry}' must be 'joint_name:value'")
            overrides[name.strip()] = v
        return overrides

    def _home_pose(self, names):
        pose = {n: self.home_overrides.get(n, HOME_VALUE) for n in names}
        # Overrides for joints that are not measured stay in, so the move is refused
        # (likely a typo) instead of silently ignoring them.
        pose.update({n: v for n, v in self.home_overrides.items() if n not in pose})
        return pose

    def _go_pose(self, names):
        if self.go_position is None:
            return {n: DEFAULT_GO_VALUE for n in names}
        return self.go_position

    @staticmethod
    def _fmt(pose):
        return ", ".join(f"{n}={v:.3f}" for n, v in pose.items())

    def _measured(self):
        """(names, positions) in joint-state array order, or (None, reason)."""
        with self._state_lock:
            states, stamps = list(self.states), list(self.stamps)
        waiting = [t for t, s in zip(self.state_topics, states) if s is None]
        if waiting:
            return None, f"Joint states not received on {', '.join(waiting)}"

        timeout = self.get_parameter("state_timeout").value
        now = time.monotonic()
        old = [t for t, s in zip(self.state_topics, stamps) if now - s > timeout]
        if old:
            return None, f"Joint states older than {timeout} s on {', '.join(old)}"

        names = [n for s in states for n in s.name]
        q = np.array([p for s in states for p in s.position], dtype=float)
        if len(names) != q.size:
            return None, f"Joint states have {len(names)} names but {q.size} positions"
        if len(set(names)) != len(names):
            return None, "Joint states contain duplicate joint names"
        if not np.all(np.isfinite(q)):
            return None, "Joint states contain non-finite positions"
        return names, q

    @staticmethod
    def _target_for(pose, names):
        """Pose as an array in the order of names, or (None, reason)."""
        missing = [n for n in names if n not in pose]
        extra = [n for n in pose if n not in set(names)]
        if missing or extra:
            parts = []
            if missing:
                parts.append(f"no pose value for measured joints {missing}")
            if extra:
                parts.append(f"pose joints not in joint states {extra}")
            return None, "; ".join(parts)
        return np.array([pose[n] for n in names], dtype=float), None

    def _publish(self, names, q):
        msg = JointState()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.name = names
        msg.position = q.tolist()
        self.pub.publish(msg)

    def _move_to(self, pose_for, label, response):
        """pose_for(names) -> {joint: value}, built from the measured joints right now."""
        if not self._move_lock.acquire(blocking=False):
            response.success = False
            response.message = "Another move is in progress"
            return response
        try:
            names, start = self._measured()
            if names is None:
                response.success = False
                response.message = start
                return response

            pose = pose_for(names)
            target, reason = self._target_for(pose, names)
            if target is None:
                response.success = False
                response.message = f"Cannot move to {label}: {reason}"
                return response

            duration = max(self.get_parameter("move_duration").value, 0.0)
            period = 1.0 / self.get_parameter("publish_rate").value

            self.get_logger().info(
                f"Moving to {label} in {duration:.1f} s "
                f"(max joint travel {np.max(np.abs(target - start)):.3f} rad)")

            # Smoothstep in time: zero velocity at both ends.
            t0 = time.monotonic()
            while rclpy.ok():
                t = 1.0 if duration == 0.0 else min((time.monotonic() - t0) / duration, 1.0)
                alpha = t * t * (3.0 - 2.0 * t)
                self._publish(names, start + alpha * (target - start))
                if t >= 1.0:
                    break
                time.sleep(period)

            if not rclpy.ok():
                response.success = False
                response.message = "Shut down during the move"
                return response

            # Wait for the arm to actually get there, keep commanding the target meanwhile.
            tol = self.get_parameter("goal_tolerance").value
            deadline = time.monotonic() + self.get_parameter("settle_timeout").value
            err = math.inf
            worst = ""
            while rclpy.ok():
                measured_names, q = self._measured()
                if measured_names is not None:
                    now_target, _ = self._target_for(pose, measured_names)
                    if now_target is not None:
                        errors = np.abs(q - now_target)
                        err = float(np.max(errors))
                        worst = measured_names[int(np.argmax(errors))]
                        if err <= tol:
                            break
                if time.monotonic() >= deadline:
                    break
                self._publish(names, target)
                time.sleep(period)

            if err <= tol:
                response.success = True
                response.message = f"Reached {label} (max error {err:.4f} rad)"
                self.get_logger().info(response.message)
            else:
                response.success = False
                response.message = (f"Did not reach {label}: max error {err:.4f} rad at "
                                    f"{worst or '?'} > goal_tolerance {tol} rad")
                self.get_logger().warn(response.message)
            return response
        finally:
            self._move_lock.release()

    # ------------------------------------------------------------------ callbacks

    def _state_cb(self, i, msg):
        with self._state_lock:
            self.states[i] = msg
            self.stamps[i] = time.monotonic()

    def move_home_cb(self, request, response):
        return self._move_to(self._home_pose, "home", response)

    def move_go_cb(self, request, response):
        return self._move_to(self._go_pose, "go pose", response)

    def set_go_position_cb(self, request, response):
        """Sets the go pose; does not move.
        - empty position: the current measured pose becomes the go pose,
        - otherwise one value per measured joint, in joint-state order (the names of each
          joint_state_topics entry in turn; the response lists them)."""
        names, current = self._measured()
        if names is None:
            response.success = False
            response.message = f"Cannot set the go pose: {current}"
            return response

        values = list(request.position)
        if not values:
            values = current.tolist()
            source = "current measured pose"
        elif len(values) != len(names) or not all(math.isfinite(v) for v in values):
            response.success = False
            response.message = (f"Expected {len(names)} finite positions in joint-state order "
                                f"{names} (or none to use the current pose), "
                                f"received {len(values)}")
            return response
        else:
            source = "request"

        self.go_position = dict(zip(names, (float(v) for v in values)))
        self.get_logger().info(f"New go pose ({source}): {self._fmt(self.go_position)}")
        response.success = True
        response.message = (f"Go position set from {source} (order: " + ", ".join(names) + ")")
        return response

    def reset_go_position_cb(self, request, response):
        self.go_position = None
        self.get_logger().info(f"Go pose reset to {DEFAULT_GO_VALUE} rad for every joint")
        response.success = True
        response.message = f"Go position reset to {DEFAULT_GO_VALUE} rad for every joint"
        return response


def main():
    rclpy.init()
    node = HomeMoveService()
    executor = MultiThreadedExecutor()
    executor.add_node(node)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == "__main__":
    main()
