#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "controller_interface/controller_interface.hpp"
#include "posveleff_controllers/motion_limiter.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "rclcpp/rclcpp.hpp"

namespace posveleff_controllers
{

/*
 * Position / velocity / effort controller with velocity and acceleration limiting.
 *
 * ~/joint_trajectory (first point) gives, per joint, the target position, the effort to send
 * (e.g. model feedforward, already in hardware sign), and in passthrough mode the velocity.
 * Each update the target is tracked by a trapezoidal profile (MotionLimiter, limits
 * max_velocity / max_acceleration), starting at rest from the measured position when the
 * first trajectory arrives, and the profile is written to the hardware:
 *   position = profile position
 *   velocity = profile velocity             (send_profile_velocity: true, MIT mode)
 *            = incoming velocity            (send_profile_velocity: false, e.g. the POS_VEL
 *                                            speed limit; dropped to 0 when stale)
 *   effort   = incoming effort
 *
 * Also published every update (URDF convention, same as the command):
 *   desired_topic       (JointTrajectory) profile p, v, a, for the dynamics feedforward
 *   command_debug_topic (JointState)      profile p, v and the effort sent, for joint_analyzer
 *   ~/debug_command_interfaces           exactly what was written to the command interfaces
 */
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

    controller_interface::CallbackReturn on_activate(
        const rclcpp_lifecycle::State & previous_state) override;

private:

    void trajectory_callback(
        const trajectory_msgs::msg::JointTrajectory::SharedPtr msg);

    // Expands a 1-element list to one value per joint. False if the size fits neither.
    bool per_joint(
        const std::vector<double> & in, const char * name, std::vector<double> & out) const;

    std::vector<std::string> joints_;

    trajectory_msgs::msg::JointTrajectory latest_traj_;

    // Set alongside latest_traj_ in trajectory_callback(); lets update()
    // detect a stalled upstream publisher instead of silently re-sending
    // an arbitrarily old setpoint forever.
    rclcpp::Time last_traj_stamp_;
    bool have_traj_ = false;

    // Guards latest_traj_, last_traj_stamp_, have_traj_ and the limits below.
    std::mutex traj_mutex_;

    // Limits per joint (changeable at runtime) and the profile state.
    std::vector<double> max_velocity_;
    std::vector<double> max_acceleration_;
    bool send_profile_velocity_ = true;
    std::vector<MotionLimiter> limiters_;
    std::vector<double> targets_;
    bool limiters_initialized_ = false;

    rclcpp::Subscription<
        trajectory_msgs::msg::JointTrajectory>::SharedPtr traj_sub_;

    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;

    // Debug tap: mirrors exactly what update() writes into
    // command_interfaces_ each cycle, for comparing against
    // ArmHardwareInterface's own debug tap on the other side of that
    // hand-off (see hw_interface's write_debug_pub_) to measure whether
    // any delay is introduced between the two.
    rclcpp::Publisher<
        sensor_msgs::msg::JointState>::SharedPtr debug_cmd_pub_;

    rclcpp::Publisher<
        trajectory_msgs::msg::JointTrajectory>::SharedPtr desired_pub_;

    rclcpp::Publisher<
        sensor_msgs::msg::JointState>::SharedPtr command_debug_pub_;
};

} // namespace posveleff_controllers
