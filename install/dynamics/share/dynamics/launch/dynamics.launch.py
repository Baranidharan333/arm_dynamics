"""Two-joint rig with the Pinocchio dynamics prediction, without gravity_compensation.

    ros2 launch dynamics dynamics.launch.py                          # real motors on can0
    ros2 launch dynamics dynamics.launch.py use_mock_hardware:=true  # no CAN
    ros2 launch dynamics dynamics.launch.py bringup:=false           # only dynamics_node, for
                                                                     # a rig that is already up

Pipeline:
    /robot_description + /joint_states
      -> dynamics_node (Pinocchio) -> /dynamics/final_torque        (prediction, measured state)
    /jnt_cmt_to_ctrl (joint_cmd's limited command q, qd)
      -> dynamics_node -> /dynamics/feedforward_torque  tau = M qdd + C qd + G along the command
    joint_cmd GUI (joint_state_publisher_gui -> /joint_cmd, one slider per joint)
      -> joint1_cmd / joint2_cmd (joint_cmd nodes: rate limit + /dynamics/feedforward_torque,
         remapped onto their /gravity_torque input)
      -> joint1_mit_controller / joint2_mit_controller (posveleff_controllers/PosVelEffController)
      -> DualJointSystem (hw_interface ArmHardwareInterface, MIT mode, motor IDs 1 and 2)

gravity_compensation's gravity_torque_node is not started: the full dynamics torque replaces
its gravity-only torque. feedforward:=false sends no torque feedforward at all.
Compare /dynamics/final_torque effort with /joint_states effort (measured) to check the model.

Start with both arms hanging straight down (q1 = q2 = 0 is the motors' zero).
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

# Per joint: hw_interface 'direction' (joint_cmd's effort_sign must match it) and the
# |effort| clamp in joint_cmd. Same values as gravity_test's dual_joint.launch.py.
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
    feedforward = LaunchConfiguration("feedforward").perform(context).lower() == "true"
    dynamics_node = Node(
        package="dynamics",
        executable="dynamics_node",
        output="screen",
        parameters=[{
            "accel_cutoff_hz": float(LaunchConfiguration("accel_cutoff_hz").perform(context)),
            # joint_cmd's limited command -> /dynamics/feedforward_torque
            "reference_topic": "/jnt_cmt_to_ctrl",
        }],
    )
    if LaunchConfiguration("bringup").perform(context).lower() != "true":
        return [dynamics_node]

    description = {"robot_description": robot_description(context)}
    test_pkg = get_package_share_directory("gravity_test")
    controllers = os.path.join(test_pkg, "config", "dual_joint_controllers.yaml")

    joint_cmd_nodes = [
        Node(
            package="gravity_test",
            executable="joint_cmd",
            name=f"{joint}_cmd",
            output="screen",
            parameters=[{
                "joint_name": joint,
                "controller_topic": f"/{joint}_mit_controller/joint_trajectory",
                # joint_cmd adds its torque input (stale > gravity_timeout -> 0) to the effort.
                "gravity_enabled": feedforward,
                "effort_sign": cfg["effort_sign"],
                "max_effort": cfg["max_effort"],
            }],
            # joint_cmd's torque input is /gravity_torque; feed it the full dynamics torque.
            remappings=[("/gravity_torque", "/dynamics/feedforward_torque")],
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
        dynamics_node,
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
            arguments=["-d", os.path.join(test_pkg, "rviz", "single_joint.rviz")],
            condition=IfCondition(LaunchConfiguration("rviz")),
        ),
    ]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("bringup", default_value="true",
                              description="Start the rig (hardware, controllers, joint_cmd); "
                                          "false = only dynamics_node."),
        DeclareLaunchArgument("use_mock_hardware", default_value="false",
                              description="Use mock_components/GenericSystem instead of the "
                                          "motors."),
        DeclareLaunchArgument("mock_initial_position", default_value="0.0",
                              description="Mock position of both joints at start [rad]."),
        DeclareLaunchArgument("feedforward", default_value="true",
                              description="Add /dynamics/feedforward_torque to joint_cmd's "
                                          "effort; false = no torque feedforward."),
        DeclareLaunchArgument("accel_cutoff_hz", default_value="10.0",
                              description="Low-pass cutoff of the estimated joint acceleration."),
        DeclareLaunchArgument("gui", default_value="true",
                              description="Start the joint_cmd slider GUI."),
        DeclareLaunchArgument("rviz", default_value="true", description="Start RViz."),
        OpaqueFunction(function=launch_setup),
    ])
