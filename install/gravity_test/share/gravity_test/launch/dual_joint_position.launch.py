"""Two-joint rig in position control (DM POS_VEL mode).

    ros2 launch gravity_test dual_joint_position.launch.py                          # real motors
    ros2 launch gravity_test dual_joint_position.launch.py use_mock_hardware:=true  # no CAN

Same rig as dual_joint.launch.py, using dual_joint_1kg_position.urdf (same robot model, but
the ros2_control block runs both motors in 'posvel' instead of 'mit'):
  - the motor's own position loop (its internal gains, not kp/kd) tracks the target,
  - the velocity field is the speed limit for the move,
  - no torque feedforward, so gravity_torque_node is not started.

Pipeline:
    joint_cmd GUI (joint_state_publisher_gui -> /joint_cmd, one slider per joint)
      -> joint1_cmd / joint2_cmd (position_cmd nodes: rate limited target + speed limit)
      -> joint1_position_controller / joint2_position_controller (PosVelEffController)
      -> DualJointSystem (hw_interface ArmHardwareInterface, POS_VEL mode, motor IDs 1 and 2)

Command vs state per joint: run joint_analyzer separately
(ros2 run joint_analyzer server) and compare /jnt_cmt_to_ctrl against /joint_states.

hw_interface switches the motor mode on activation, so after this launch the motors stay in
POS_VEL until dual_joint.launch.py (MIT) is started again, which switches them back.
Start with both arms hanging straight down (q1 = q2 = 0 is the motors' zero).
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

# Per joint: hw_interface 'direction' from the URDF; position_cmd uses it to send the speed
# limit with the right sign.
JOINTS = {
    "joint1": {"direction": -1.0},
    "joint2": {"direction": -1.0},
}


def robot_description(context):
    path = os.path.join(
        get_package_share_directory("single_joint_description"), "urdf",
        "dual_joint_1kg_position.urdf")
    with open(path) as f:
        urdf = f.read()

    if LaunchConfiguration("use_mock_hardware").perform(context).lower() == "true":
        # mock_components/GenericSystem mirrors the position command into the state.
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
    controllers = os.path.join(pkg, "config", "dual_joint_position_controllers.yaml")
    max_velocity = float(LaunchConfiguration("max_velocity").perform(context))
    speed_limit = float(LaunchConfiguration("speed_limit").perform(context))

    joint_cmd_nodes = [
        Node(
            package="gravity_test",
            executable="position_cmd",
            name=f"{joint}_cmd",
            output="screen",
            parameters=[{
                "joint_name": joint,
                "controller_topic": f"/{joint}_position_controller/joint_trajectory",
                "direction": cfg["direction"],
                "max_velocity": max_velocity,
                "speed_limit": speed_limit,
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
            arguments=["joint_state_broadcaster", "joint1_position_controller",
                       "joint2_position_controller", "--controller-manager",
                       "/controller_manager"],
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
        DeclareLaunchArgument("max_velocity", default_value="1.0",
                              description="joint_cmd ramp speed of the target [rad/s]."),
        DeclareLaunchArgument("speed_limit", default_value="2.0",
                              description="POS_VEL speed limit sent to the motors [rad/s]; "
                                          "keep it above max_velocity so the motor keeps up "
                                          "with the ramp."),
        DeclareLaunchArgument("gui", default_value="true",
                              description="Start the joint_cmd slider GUI."),
        DeclareLaunchArgument("rviz", default_value="true", description="Start RViz."),
        OpaqueFunction(function=launch_setup),
    ])
