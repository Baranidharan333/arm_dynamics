#include "posveleff_controllers/posveleff_controller.hpp"

#include <algorithm>
#include <cmath>
#include <pluginlib/class_list_macros.hpp>

namespace posveleff_controllers
{

// Above this age, latest_traj_ is treated as stale and update() logs a
// warning instead of silently re-sending it forever. Generous relative to
// the ~5ms publish period of a healthy upstream (joint_cmd.py at 200Hz)
// so ordinary DDS/scheduling jitter doesn't false-positive, while still
// catching a genuinely stalled publisher long before the hardware layer's
// own ~0.5s LOST-motor timeout would.
constexpr double kStaleTrajThresholdSec = 0.1;

// Longest step the profile integrates in one update; a longer gap (scheduling hiccup) is
// treated as this long so the profile does not jump.
constexpr double kMaxStepSec = 0.05;

controller_interface::CallbackReturn
PosVelEffController::on_init()
{
    auto_declare<std::vector<std::string>>(
        "joints",
        std::vector<std::string>{});

    // One value for all joints, or one per joint. Changeable at runtime.
    auto_declare<std::vector<double>>(
        "max_velocity", std::vector<double>{3.0});        // rad/s
    auto_declare<std::vector<double>>(
        "max_acceleration", std::vector<double>{6.0});    // rad/s^2

    auto_declare<bool>("send_profile_velocity", true);
    auto_declare<std::string>("desired_topic", "/joint_desired");
    auto_declare<std::string>("command_debug_topic", "/jnt_cmt_to_ctrl");

    return controller_interface::CallbackReturn::SUCCESS;
}

bool PosVelEffController::per_joint(
    const std::vector<double> & in, const char * name, std::vector<double> & out) const
{
    const auto n = joints_.size();
    if (in.size() == 1) {
        out.assign(n, in.front());
    } else if (in.size() == n) {
        out = in;
    } else {
        RCLCPP_ERROR(
            get_node()->get_logger(),
            "%s has %zu values; need 1 or one per joint (%zu)", name, in.size(), n);
        return false;
    }
    for (double v : out) {
        if (!(v > 0.0)) {
            RCLCPP_ERROR(get_node()->get_logger(), "%s values must be > 0", name);
            return false;
        }
    }
    return true;
}

controller_interface::CallbackReturn
PosVelEffController::on_configure(
    const rclcpp_lifecycle::State &)
{
    auto node = get_node();

    joints_ =
        node->get_parameter("joints")
            .as_string_array();

    if (!per_joint(node->get_parameter("max_velocity").as_double_array(),
                   "max_velocity", max_velocity_) ||
        !per_joint(node->get_parameter("max_acceleration").as_double_array(),
                   "max_acceleration", max_acceleration_))
    {
        return controller_interface::CallbackReturn::ERROR;
    }
    send_profile_velocity_ = node->get_parameter("send_profile_velocity").as_bool();

    limiters_.assign(joints_.size(), MotionLimiter{});
    targets_.assign(joints_.size(), 0.0);

    RCLCPP_INFO(
        node->get_logger(),
        "Loaded %zu joints, velocity sent: %s",
        joints_.size(),
        send_profile_velocity_ ? "profile" : "passthrough");
    for (size_t i = 0; i < joints_.size(); ++i)
    {
        RCLCPP_INFO(
            node->get_logger(), "  %s: max_velocity %.3f rad/s, max_acceleration %.3f rad/s^2",
            joints_[i].c_str(), max_velocity_[i], max_acceleration_[i]);
    }

    param_cb_ = node->add_on_set_parameters_callback(
        [this](const std::vector<rclcpp::Parameter> & params)
        {
            rcl_interfaces::msg::SetParametersResult result;
            result.successful = true;
            for (const auto & p : params)
            {
                const auto & name = p.get_name();
                if (name != "max_velocity" && name != "max_acceleration") continue;

                std::vector<double> values;
                if (p.get_type() != rclcpp::ParameterType::PARAMETER_DOUBLE_ARRAY ||
                    !per_joint(p.as_double_array(), name.c_str(), values))
                {
                    result.successful = false;
                    result.reason = name + ": need 1 or one per joint value(s), all > 0";
                    return result;
                }
                std::lock_guard<std::mutex> lock(traj_mutex_);
                (name == "max_velocity" ? max_velocity_ : max_acceleration_) = values;
                RCLCPP_INFO(get_node()->get_logger(), "[param] %s updated", name.c_str());
            }
            return result;
        });

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

    desired_pub_ =
        node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
            node->get_parameter("desired_topic").as_string(), 10);

    command_debug_pub_ =
        node->create_publisher<sensor_msgs::msg::JointState>(
            node->get_parameter("command_debug_topic").as_string(), 10);

    RCLCPP_INFO(
        node->get_logger(),
        "MIT controller configured");

    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn
PosVelEffController::on_activate(
    const rclcpp_lifecycle::State &)
{
    std::lock_guard<std::mutex> lock(traj_mutex_);

    // Wait for a new trajectory and restart the profile from the measured position, so a
    // setpoint from before a deactivate is never replayed.
    latest_traj_ = trajectory_msgs::msg::JointTrajectory();
    have_traj_ = false;
    limiters_initialized_ = false;

    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration
PosVelEffController::command_interface_configuration() const
{
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
    // Measured position: the profile starts from it.
    controller_interface::InterfaceConfiguration cfg;

    cfg.type =
        controller_interface::interface_configuration_type::INDIVIDUAL;

    for (const auto & joint : joints_)
    {
        cfg.names.push_back(joint + "/position");
    }

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
    const rclcpp::Duration & period)
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
                "No fresh /joint_trajectory in %.3fs - holding the last "
                "target (upstream publisher may have stalled)",
                age_sec);
        }
    }

    const auto n = joints_.size();

    // First trajectory: start the profile at rest from where the joints really are.
    if (!limiters_initialized_)
    {
        for (size_t i = 0; i < n; ++i)
        {
            const auto q = state_interfaces_[i].get_optional();
            if (!q || !std::isfinite(*q))
            {
                RCLCPP_WARN_THROTTLE(
                    get_node()->get_logger(),
                    *get_node()->get_clock(),
                    1000,
                    "No measured position for %s yet; not commanding",
                    joints_[i].c_str());
                return controller_interface::return_type::OK;
            }
            limiters_[i].Reset(*q);
            targets_[i] = *q;
        }
        limiters_initialized_ = true;
        RCLCPP_INFO(get_node()->get_logger(), "Profile started from the measured position");
    }

    const double dt = std::min(period.seconds(), kMaxStepSec);

    auto & point =
        latest_traj_.points.front();

    sensor_msgs::msg::JointState dbg;
    dbg.header.stamp = time;
    dbg.name = joints_;

    trajectory_msgs::msg::JointTrajectory desired;
    desired.header.stamp = time;
    desired.joint_names = joints_;
    trajectory_msgs::msg::JointTrajectoryPoint dp;

    for (size_t i = 0; i < n; i++)
    {
        if (i < point.positions.size() && std::isfinite(point.positions[i]))
            targets_[i] = point.positions[i];

        auto & lim = limiters_[i];
        lim.Step(targets_[i], dt, max_velocity_[i], max_acceleration_[i]);

        const double p = lim.pos;
        double v = 0.0;
        double e = 0.0;

        if (send_profile_velocity_)
        {
            v = lim.vel;
        }
        // Passthrough (POS_VEL speed limit): drop it once stale, so a frozen
        // upstream does not keep the motor moving.
        else if (!stale && i < point.velocities.size())
        {
            v = point.velocities[i];
        }

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

        dp.positions.push_back(lim.pos);
        dp.velocities.push_back(lim.vel);
        dp.accelerations.push_back(lim.acc);
    }

    debug_cmd_pub_->publish(dbg);

    desired.points.push_back(dp);
    desired_pub_->publish(desired);

    // Profile (not the passthrough speed limit) for joint_analyzer.
    sensor_msgs::msg::JointState cmd = dbg;
    cmd.velocity = dp.velocities;
    command_debug_pub_->publish(cmd);

    return controller_interface::return_type::OK;
}

} // namespace posveleff_controllers

PLUGINLIB_EXPORT_CLASS(
    posveleff_controllers::PosVelEffController,
    controller_interface::ControllerInterface)
