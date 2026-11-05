"""Single-joint gravity compensation rig.

    ros2 launch gravity_test single_joint.launch.py                          # real motor on can0
    ros2 launch gravity_test single_joint.launch.py use_mock_hardware:=true  # no CAN

Pipeline:
    /robot_description + /joint_states
      -> gravity_torque_node (gravity_compensation, KDL) -> /gravity_torque
    joint_cmd GUI (joint_state_publisher_gui -> /joint_cmd)
      -> joint_cmd node (rate limit + /gravity_torque feedforward)
      -> single_joint_mit_controller (posveleff_controllers/PosVelEffController)
      -> SingleJointSystem (hw_interface ArmHardwareInterface, MIT mode)

Start with the arm hanging straight down: hw_interface has no zero offset, so the motor's
zero must already be set at that pose (q = 0 in the URDF).
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def robot_description(context):
    path = os.path.join(
        get_package_share_directory("single_joint_description"), "urdf", "single_joint_1kg.urdf")
    with open(path) as f:
        urdf = f.read()

    if LaunchConfiguration("use_mock_hardware").perform(context).lower() == "true":
        # mock_components/GenericSystem mirrors the position command into the state, so the
        # joint follows /joint_cmd and the gravity torque can be checked without a motor.
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
    controllers = os.path.join(pkg, "config", "single_joint_controllers.yaml")

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
            arguments=["joint_state_broadcaster", "single_joint_mit_controller",
                       "--controller-manager", "/controller_manager"],
        ),
        Node(
            package="gravity_compensation",
            executable="gravity_torque_node",
            output="screen",
            parameters=[os.path.join(pkg, "config", "single_joint_gravity_torque.yaml")],
            condition=IfCondition(LaunchConfiguration("gravity")),
        ),
        Node(
            package="gravity_test",
            executable="joint_cmd",
            output="screen",
            parameters=[{
                "gravity_enabled": LaunchConfiguration("gravity").perform(context).lower()
                == "true",
            }],
        ),
        # Sliders publish position targets on /joint_cmd instead of /joint_states.
        Node(
            package="joint_state_publisher_gui",
            executable="joint_state_publisher_gui",
            name="joint_cmd_publisher_gui",
            remappings=[("joint_states", "/joint_cmd")],
            condition=IfCondition(LaunchConfiguration("gui")),
        ),
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
                              description="Use mock_components/GenericSystem instead of the motor."),
        DeclareLaunchArgument("mock_initial_position", default_value="0.0",
                              description="Mock joint position at start [rad]."),
        DeclareLaunchArgument("gravity", default_value="true",
                              description="Run gravity_torque_node and add its torque in "
                                          "joint_cmd."),
        DeclareLaunchArgument("gui", default_value="true",
                              description="Start the joint_cmd slider GUI."),
        DeclareLaunchArgument("rviz", default_value="true", description="Start RViz."),
        OpaqueFunction(function=launch_setup),
    ])
