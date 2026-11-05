"""Show the single-joint rig in RViz, no hardware.

    ros2 launch single_joint_description display.launch.py

robot_state_publisher publishes the URDF, joint_state_publisher_gui moves joint1 with a slider,
RViz shows the model and TF. For the real rig use gravity_test's single_joint.launch.py.
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg = get_package_share_directory("single_joint_description")
    with open(os.path.join(pkg, "urdf", "single_joint_1kg.urdf")) as f:
        robot_description = f.read()

    return LaunchDescription([
        DeclareLaunchArgument("gui", default_value="true",
                              description="Start the joint slider GUI."),
        DeclareLaunchArgument("rviz", default_value="true", description="Start RViz."),
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            parameters=[{"robot_description": robot_description}],
        ),
        Node(
            package="joint_state_publisher_gui",
            executable="joint_state_publisher_gui",
            condition=IfCondition(LaunchConfiguration("gui")),
        ),
        Node(
            package="rviz2",
            executable="rviz2",
            arguments=["-d", os.path.join(pkg, "rviz", "display.rviz")],
            condition=IfCondition(LaunchConfiguration("rviz")),
        ),
    ])
