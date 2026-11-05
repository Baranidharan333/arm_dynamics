#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "controller_interface/controller_interface.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "rclcpp/rclcpp.hpp"

namespace posveleff_controllers
{

class PosVelEffController
    : public controller_interface::ControllerInterface
{
public:
    controller_interface::CallbackReturn on_init() override;

    controller_interface::InterfaceConfiguration
    command_interface_configuration() const override;

    controller_interface::InterfaceConfiguration
    state_interface_configuration() const override;

    controller_interface::return_type update(
        const rclcpp::Time & time,
        const rclcpp::Duration & period) override;

    controller_interface::CallbackReturn on_configure(
        const rclcpp_lifecycle::State & previous_state) override;

private:

    void trajectory_callback(
        const trajectory_msgs::msg::JointTrajectory::SharedPtr msg);

    std::vector<std::string> joints_;

    trajectory_msgs::msg::JointTrajectory latest_traj_;

    // Set alongside latest_traj_ in trajectory_callback(); lets update()
    // detect a stalled upstream publisher instead of silently re-sending
    // an arbitrarily old setpoint forever.
    rclcpp::Time last_traj_stamp_;
    bool have_traj_ = false;

    std::mutex traj_mutex_;

    rclcpp::Subscription<
        trajectory_msgs::msg::JointTrajectory>::SharedPtr traj_sub_;

    // Debug tap: mirrors exactly what update() writes into
    // command_interfaces_ each cycle, for comparing against
    // ArmHardwareInterface's own debug tap on the other side of that
    // hand-off (see hw_interface's write_debug_pub_) to measure whether
    // any delay is introduced between the two.
    rclcpp::Publisher<
        sensor_msgs::msg::JointState>::SharedPtr debug_cmd_pub_;
};

} // namespace posveleff_controllers