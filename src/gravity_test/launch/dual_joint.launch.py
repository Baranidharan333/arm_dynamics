"""Two-joint gravity compensation rig.

    ros2 launch gravity_test dual_joint.launch.py                          # real motors on can0
    ros2 launch gravity_test dual_joint.launch.py use_mock_hardware:=true  # no CAN

Command vs state per joint: run joint_analyzer separately
(ros2 run joint_analyzer server) and compare /jnt_cmt_to_ctrl against /joint_states.

Pipeline:
    /robot_description + /joint_states
      + /joint_desired (limited targets p, v, a from joint1_cmd / joint2_cmd)
      -> gravity_compensation_node (dynamics, KDL)
      -> /feedforward_torque = M(q_d) ddq_d + C(q_d, dq_d) dq_d + G(q) (joint1, joint2)
    joint_cmd GUI (joint_state_publisher_gui -> /joint_cmd, one slider per joint)
      -> joint1_cmd / joint2_cmd (joint_cmd nodes: rate limit + /feedforward_torque)
      -> joint1_mit_controller / joint2_mit_controller (posveleff_controllers/PosVelEffController)
      -> DualJointSystem (hw_interface ArmHardwareInterface, MIT mode, motor IDs 1 and 2)

Start with both arms hanging straight down: hw_interface has no zero offset, so both motors'
zeros must already be set at that pose (q1 = q2 = 0 in the URDF).
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

# Per joint: hw_interface 'direction' (joint_cmd's effort_sign must match it) and the
# |effort| clamp in joint_cmd. Peak gravity torque is ~12.4 Nm on joint1, ~4.9 Nm on joint2;
# inertia adds up to ~M * max_acceleration on top while the target accelerates.
JOINTS = {
    "joint1": {"effort_sign": -1.0, "max_effort": 13.0},
    "joint2": {"effort_sign": -1.0, "max_effort": 5.5},
}


def robot_description(context):
    path = os.path.join(
        get_package_share_directory("single_joint_description"), "urdf", "dual_joint_1kg.urdf")
    with open(path) as f:
        urdf = f.read()

    if LaunchConfiguration("use_mock_hardware").perform(context).lower() == "true":
        # mock_components/GenericSystem mirrors the position command into the state, so the
        # joints follow /joint_cmd and the gravity torque can be checked without motors.
        urdf = urdf.replace(
            "<plugin>arm_hardware/ArmHardwareInterface</plugin>",
            "<plugin>mock_components/GenericSystem</plugin>")
        urdf = urdf.replace(
            '<state_interface name="position"/>',
            '<state_interface name="position"><param name="initial_value">'
            + LaunchConfiguration("mock_initial_position").perform(context)
            + "</param></state_interface>")
    return urdf


def launch_setup(context):
    description = {"robot_description": robot_description(context)}
    pkg = get_package_share_directory("gravity_test")
    controllers = os.path.join(pkg, "config", "dual_joint_controllers.yaml")
    gravity = LaunchConfiguration("gravity").perform(context).lower() == "true"

    joint_cmd_nodes = [
        Node(
            package="gravity_test",
            executable="joint_cmd",
            name=f"{joint}_cmd",
            output="screen",
            parameters=[{
                "joint_name": joint,
                "controller_topic": f"/{joint}_mit_controller/joint_trajectory",
                "feedforward_enabled": gravity,
                "effort_sign": cfg["effort_sign"],
                "max_effort": cfg["max_effort"],
            }],
        )
        for joint, cfg in JOINTS.items()
    ]

    return [
        # Always brings up can0 (needs VCAN_SUDO_PASSWORD).
        Node(
            package="hw_interface",
            executable="can_bridge_node",
            output="screen",
        ),
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            parameters=[description],
        ),
        Node(
            package="controller_manager",
            executable="ros2_control_node",
            parameters=[controllers],
            remappings=[("~/robot_description", "/robot_description")],
            output="screen",
        ),
        Node(
            package="controller_manager",
            executable="spawner",
            arguments=["joint_state_broadcaster", "joint1_mit_controller",
                       "joint2_mit_controller", "--controller-manager", "/controller_manager"],
        ),
        Node(
            package="dynamics",
            executable="gravity_compensation_node",
            output="screen",
            # Feedforward tuning (ramp, scales) first, then this rig's chain.
            parameters=[
                os.path.join(get_package_share_directory("dynamics"), "config",
                             "feedforward.yaml"),
                os.path.join(pkg, "config", "dual_joint_gravity_torque.yaml"),
            ],
            condition=IfCondition(LaunchConfiguration("gravity")),
        ),
        *joint_cmd_nodes,
        # Sliders publish position targets on /joint_cmd instead of /joint_states.
        Node(
            package="joint_state_publisher_gui",
            executable="joint_state_publisher_gui",
            name="joint_cmd_publisher_gui",
            remappings=[("joint_states", "/joint_cmd")],
            condition=IfCondition(LaunchConfiguration("gui")),
        ),
        # single_joint.rviz only shows /robot_description and TF from base_link, so it fits.
        Node(
            package="rviz2",
            executable="rviz2",
            arguments=["-d", os.path.join(pkg, "rviz", "single_joint.rviz")],
            condition=IfCondition(LaunchConfiguration("rviz")),
        ),
    ]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("use_mock_hardware", default_value="false",
                              description="Use mock_components/GenericSystem instead of the "
                                          "motors."),
        DeclareLaunchArgument("mock_initial_position", default_value="0.0",
                              description="Mock position of both joints at start [rad]."),
        DeclareLaunchArgument("gravity", default_value="true",
                              description="Run gravity_compensation_node and add its feedforward torque in "
                                          "joint_cmd."),
        DeclareLaunchArgument("gui", default_value="true",
                              description="Start the joint_cmd slider GUI."),
        DeclareLaunchArgument("rviz", default_value="true", description="Start RViz."),
        OpaqueFunction(function=launch_setup),
    ])
