#include "posveleff_controllers/posveleff_controller.hpp"

#include <pluginlib/class_list_macros.hpp>

namespace posveleff_controllers
{

// Above this age, latest_traj_ is treated as stale and update() logs a
// warning instead of silently re-sending it forever. Generous relative to
// the ~2ms publish period of a healthy upstream (joint_cmd.py's
// JointCommandLimiter at 500Hz) so ordinary DDS/scheduling jitter doesn't
// false-positive, while still catching a genuinely stalled publisher long
// before the hardware layer's own ~0.5s LOST-motor timeout would.
constexpr double kStaleTrajThresholdSec = 0.1;

controller_interface::CallbackReturn
PosVelEffController::on_init()
{
        auto_declare<std::vector<std::string>>(
            "joints",
            std::vector<std::string>{});

    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn
PosVelEffController::on_configure(
    const rclcpp_lifecycle::State &)
{
    auto node = get_node();

    joints_ =
        get_node()->get_parameter("joints")
            .as_string_array();

    RCLCPP_INFO(
        get_node()->get_logger(),
        "Loaded %zu joints",
        joints_.size());

    traj_sub_ =
        node->create_subscription<
            trajectory_msgs::msg::JointTrajectory>(
            "~/joint_trajectory",
            10,
            std::bind(
                &PosVelEffController::trajectory_callback,
                this,
                std::placeholders::_1));

    debug_cmd_pub_ =
        node->create_publisher<sensor_msgs::msg::JointState>(
            "~/debug_command_interfaces", 10);

    RCLCPP_INFO(
        node->get_logger(),
        "MIT controller configured");

    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration
PosVelEffController::command_interface_configuration() const
{
        RCLCPP_INFO(
        rclcpp::get_logger("MITController"),
        "command_interface_configuration() joints=%zu",
        joints_.size());
    controller_interface::InterfaceConfiguration cfg;

    cfg.type =
        controller_interface::interface_configuration_type::INDIVIDUAL;

    for (const auto & joint : joints_)
    {
        cfg.names.push_back(joint + "/position");
        cfg.names.push_back(joint + "/velocity");
        cfg.names.push_back(joint + "/effort");
    }

    return cfg;
}

controller_interface::InterfaceConfiguration
PosVelEffController::state_interface_configuration() const
{
    controller_interface::InterfaceConfiguration cfg;

    cfg.type =
        controller_interface::interface_configuration_type::NONE;

    return cfg;
}

void PosVelEffController::trajectory_callback(
    const trajectory_msgs::msg::JointTrajectory::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(traj_mutex_);

    latest_traj_ = *msg;
    last_traj_stamp_ = get_node()->get_clock()->now();
    have_traj_ = true;
}

controller_interface::return_type
PosVelEffController::update(
    const rclcpp::Time & time,
    const rclcpp::Duration &)
{
    std::lock_guard<std::mutex> lock(traj_mutex_);

    if (latest_traj_.points.empty())
    {
        return controller_interface::return_type::OK;
    }

    bool stale = false;

    if (have_traj_)
    {
        const double age_sec = (time - last_traj_stamp_).seconds();

        if (age_sec > kStaleTrajThresholdSec)
        {
            stale = true;

            RCLCPP_WARN_THROTTLE(
                get_node()->get_logger(),
                *get_node()->get_clock(),
                1000,
                "No fresh /joint_trajectory in %.3fs - still commanding "
                "last received setpoint (upstream publisher may have "
                "stalled)",
                age_sec);
        }
    }

    auto & point =
        latest_traj_.points.front();

    sensor_msgs::msg::JointState dbg;
    dbg.header.stamp = time;
    dbg.name = joints_;

    for (size_t i = 0; i < joints_.size(); i++)
    {
        double p = 0.0;
        double v = 0.0;
        double e = 0.0;

        if (i < point.positions.size())
            p = point.positions[i];

        // Hold the last position but drop its velocity feedforward once
        // stale: re-sending a mid-motion velocity against a frozen position
        // makes the motor push past the setpoint and get pulled back (seen
        // as the gripper twitching open/closed during upstream stalls).
        if (!stale && i < point.velocities.size())
            v = point.velocities[i];

        if (i < point.effort.size())
            e = point.effort[i];

        if (!command_interfaces_[3 * i + 0].set_value(p) ||
            !command_interfaces_[3 * i + 1].set_value(v) ||
            !command_interfaces_[3 * i + 2].set_value(e))
        {
            RCLCPP_WARN_THROTTLE(
                get_node()->get_logger(),
                *get_node()->get_clock(),
                1000,
                "Failed to set command for joint %s", joints_[i].c_str());
        }

        dbg.position.push_back(p);
        dbg.velocity.push_back(v);
        dbg.effort.push_back(e);
    }

    debug_cmd_pub_->publish(dbg);

    return controller_interface::return_type::OK;
}

} // namespace posveleff_controllers

PLUGINLIB_EXPORT_CLASS(
    posveleff_controllers::PosVelEffController,
    controller_interface::ControllerInterface)