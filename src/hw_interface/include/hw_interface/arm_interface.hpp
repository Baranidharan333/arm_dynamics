#ifndef ARM_HARDWARE_INTERFACE_HPP
#define ARM_HARDWARE_INTERFACE_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "hardware_interface/system_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp/duration.hpp"
#include "hw_interface/driver.hpp"
#include "hw_interface/socketcan_transport.hpp"

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "hw_interface/msg/motor_status_array.hpp"
#include "hw_interface/srv/set_motor_gains.hpp"

#include "sensor_msgs/msg/joint_state.hpp"

namespace arm_hardware {

class ArmHardwareInterface : public hardware_interface::SystemInterface
{
public:
    // -------- Lifecycle --------
    hardware_interface::CallbackReturn on_init(
        const hardware_interface::HardwareComponentInterfaceParams & params) override;

    hardware_interface::CallbackReturn on_configure(
        const rclcpp_lifecycle::State & previous_state) override;

    hardware_interface::CallbackReturn on_activate(
        const rclcpp_lifecycle::State & previous_state) override;

    hardware_interface::CallbackReturn on_deactivate(
        const rclcpp_lifecycle::State & previous_state) override;

    hardware_interface::CallbackReturn on_shutdown(
        const rclcpp_lifecycle::State & previous_state) override;

    hardware_interface::CallbackReturn on_error(
        const rclcpp_lifecycle::State & previous_state) override;

    // -------- IO --------
    hardware_interface::return_type read(
        const rclcpp::Time & time,
        const rclcpp::Duration & period) override;

    hardware_interface::return_type write(
        const rclcpp::Time & time,
        const rclcpp::Duration & period) override;

    // -------- Interfaces --------
    std::vector<hardware_interface::StateInterface>
    export_state_interfaces() override;

    std::vector<hardware_interface::CommandInterface>
    export_command_interfaces() override;

private:

    // ---------- Motor state machine ----------
    enum class MotorState
    {
        NORMAL,
        LOST,
        RECOVERING,
        HOLDING
    };
    std::vector<bool> joint_is_prismatic_;

    // ---------- Per-joint control mode ----------
    // Set from the "control_modes" hardware parameter (comma separated
    // "mit"/"posvel" per joint). Defaults to MIT for every joint when
    // the parameter is absent, so existing URDFs keep working unchanged.
    enum class HardwareControlMode
    {
        MIT,
        POSVEL
    };
    std::vector<HardwareControlMode> control_modes_;

    double gripper_joint_max_ = 0.044;

    double gripper_motor_max_rad_ = -1.8370; // withot rubber padding = -1.81
    // -------- Flags --------
    bool initialized_ = false;

    // -------- Parameters --------
    std::string can_interface_;
    std::string arm_name_;
    std::vector<int> motor_ids_;
    std::vector<float> kp_;
    std::vector<float> kd_;

    // -------- MIT mapping ranges (must match motor registers PMAX/VMAX/TMAX) --------
    // From the "p_max"/"v_max"/"t_max" hardware parameters. Defaults are the
    // DM4310 values (12.5 / 30 / 10); a DM4340 needs 12.5 / 10 / 28 (or 8 on 24 V).
    std::vector<float> p_max_;
    std::vector<float> v_max_;
    std::vector<float> t_max_;

    // -------- Directions --------
    std::vector<int> direction_;

    // -------- Mapping --------
    std::unordered_map<int, size_t> id_to_index_;

    // -------- Communication --------
    std::unique_ptr<SocketCANTransport> transport_;
    std::vector<std::unique_ptr<damiao::DamiaoCanDriver>> motors_;

    // -------- Buffers --------

    // Position commands
    std::vector<double> hw_commands_;

    // NEW: Velocity commands
    std::vector<double> hw_velocity_commands_;

    // NEW: Torque commands
    std::vector<double> hw_torque_commands_;

    // States
    std::vector<double> hw_positions_;
    std::vector<double> hw_velocities_;
    std::vector<double> hw_efforts_;
    
    // ---------- Recovery ----------
    std::vector<MotorState> motor_states_;
    std::vector<int> missed_cycles_;
    std::vector<double> hold_positions_;
    double last_debug_print_time_sec_ = 0.0;
    std::vector<int> holding_stable_counts_;
    std::vector<double> recovery_command_snapshot_;

    // Recovery service
    // --------------------------------------------------
    rclcpp::Node::SharedPtr recovery_node_;

    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr
        recovery_service_;

    bool recovery_requested_ = false;

    // Guards against registering the rclcpp::on_shutdown()
    // force-disable hook more than once if on_configure() runs again
    // (e.g. reconfigure after error recovery).
    bool shutdown_hook_registered_ = false;
    void recovery_service_callback(
    const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response);

    rclcpp::Publisher<
    hw_interface::msg::MotorStatusArray>::SharedPtr
        motor_status_pub_;
    rclcpp::Service<hw_interface::srv::SetMotorGains>::SharedPtr
        gain_service_;
    std::vector<int> motor_errors_;
    std::vector<float> motor_mos_temp_;
    std::vector<float> motor_rotor_temp_;
    void set_motor_gain_callback(
        const std::shared_ptr<
            hw_interface::srv::SetMotorGains::Request> request,
        std::shared_ptr<
            hw_interface::srv::SetMotorGains::Response> response);
    rclcpp::Publisher<
        sensor_msgs::msg::JointState>::SharedPtr
            joint_state_pub_;

    // Debug tap: the raw hw_commands_/hw_velocity_commands_/hw_torque_commands_
    // write() sees at the top of each cycle, before any per-motor state-machine
    // branch (LOST/RECOVERING/HOLDING/NORMAL) or direction_/prismatic transform
    // is applied. Compare against posveleff_controllers' debug_cmd_pub_ (same
    // values, written one hop upstream) to measure hand-off delay, if any.
    rclcpp::Publisher<
        sensor_msgs::msg::JointState>::SharedPtr
            write_debug_pub_;

            // =====================================
        // ADD THESE HERE
        // =====================================

        double motor_rad_to_linear(double rad);

        double linear_to_motor_rad(double meter);

        // Shared by on_deactivate/on_shutdown/on_error: the lifecycle
        // state machine can transition ACTIVE -> FINALIZED directly
        // (e.g. process termination), which calls on_shutdown instead
        // of on_deactivate, so both must disable the motors.
        void disable_all_motors();
};

} // namespace arm_hardware

#endif