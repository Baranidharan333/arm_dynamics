from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():

    host_arg = DeclareLaunchArgument(
        "host",
        default_value="0.0.0.0",
        description="Host/interface both web servers bind to",
    )
    server_port_arg = DeclareLaunchArgument(
        "server_port",
        default_value="8130",
        description="Port for the joint_cmd vs joint_states comparison UI",
    )
    plot_port_arg = DeclareLaunchArgument(
        "plot_port",
        default_value="8131",
        description="Port for the generic live topic-field plotter",
    )

    host = LaunchConfiguration("host")
    server_port = LaunchConfiguration("server_port")
    plot_port = LaunchConfiguration("plot_port")

    server_node = Node(
        package="joint_analyzer",
        executable="server",
        name="joint_analyzer_server",
        output="screen",
        arguments=["--host", host, "--port", server_port],
    )

    plot_server_node = Node(
        package="joint_analyzer",
        executable="plot_server",
        name="joint_analyzer_plot_server",
        output="screen",
        arguments=["--host", host, "--port", plot_port],
    )

    return LaunchDescription([
        host_arg,
        server_port_arg,
        plot_port_arg,
        server_node,
        plot_server_node,
    ])
