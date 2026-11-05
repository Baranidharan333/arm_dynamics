#include "hw_interface/arm_interface.hpp"
#include "rclcpp/rclcpp.hpp"
#include <thread>
#include <algorithm>
#include <sstream>
template <typename T>
std::vector<T> parse_csv(const std::string & csv)
{
    std::vector<T> result;
    std::stringstream ss(csv);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if constexpr (std::is_same<T, int>::value)
            result.push_back(std::stoi(item));
        else
            result.push_back(std::stof(item));
    }
    return result;
}
namespace arm_hardware {

hardware_interface::CallbackReturn ArmHardwareInterface::on_init(
    const hardware_interface::HardwareInfo & info)
{
    // --------------------------------------------------
    // Base class init
    // --------------------------------------------------
    if (SystemInterface::on_init(info) !=
        hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    info_ = info;

    // --------------------------------------------------
    // CAN interface
    // --------------------------------------------------
    can_interface_ =
        info_.hardware_parameters.at("can_interface");

    // --------------------------------------------------
    // Parse motor IDs
    // --------------------------------------------------
    motor_ids_ =
        parse_csv<int>(
            info_.hardware_parameters.at("motor_ids"));

    const size_t n = motor_ids_.size();

    // --------------------------------------------------
    // Initialize motor state machine
    // --------------------------------------------------
    motor_states_.assign(n, MotorState::NORMAL);


    // -------- Hold positions --------
    hold_positions_.assign(n, 0.0);
    recovery_command_snapshot_.assign(n, 0.0);



    // --------------------------------------------------
    // Missed cycle counters
    // --------------------------------------------------
    missed_cycles_.assign(n, 0);
    holding_stable_counts_.assign(n, 0);

    // --------------------------------------------------
    // Validate joint count
    // --------------------------------------------------
    if (info_.joints.size() != n)
    {
        RCLCPP_ERROR(
            rclcpp::get_logger("ArmHardware"),
            "Mismatch: joints=%zu motors=%zu",
            info_.joints.size(),
            n);

        return hardware_interface::CallbackReturn::ERROR;
    }

    // --------------------------------------------------
    // Parse KP
    // --------------------------------------------------
    if (info_.hardware_parameters.count("kp"))
    {
        kp_ =
            parse_csv<float>(
                info_.hardware_parameters.at("kp"));
    }
    else
    {
        kp_ = std::vector<float>(n, 15.0f);
    }

    // --------------------------------------------------
    // Parse KD
    // --------------------------------------------------
    if (info_.hardware_parameters.count("kd"))
    {
        kd_ =
            parse_csv<float>(
                info_.hardware_parameters.at("kd"));
    }
    else
    {
        kd_ = std::vector<float>(n, 1.0f);
    }

    // --------------------------------------------------
    // Safety checks
    // --------------------------------------------------
    if (kp_.size() != n || kd_.size() != n)
    {
        RCLCPP_ERROR(
            rclcpp::get_logger("ArmHardware"),
            "Mismatch: motor_ids=%zu kp=%zu kd=%zu",
            n,
            kp_.size(),
            kd_.size());

        return hardware_interface::CallbackReturn::ERROR;
    }

    // --------------------------------------------------
    // Initialize hardware buffers
    // --------------------------------------------------
    hw_positions_.assign(n, 0.0);
    hw_velocities_.assign(n, 0.0);
    hw_efforts_.assign(n, 0.0);
    hw_commands_.assign(n, 0.0);

    // --------------------------------------------------
    // Debug print
    // --------------------------------------------------
    for (size_t i = 0; i < n; i++)
    {
        RCLCPP_INFO(
            rclcpp::get_logger("ArmHardware"),
            "Motor[%zu] ID=%d kp=%.2f kd=%.2f",
            i,
            motor_ids_[i],
            kp_[i],
            kd_[i]);
    }

    RCLCPP_INFO(
        rclcpp::get_logger("ArmHardware"),
        "Hardware interface initialized successfully");

    return hardware_interface::CallbackReturn::SUCCESS;
}
// -------- STATE INTERFACES --------
std::vector<hardware_interface::StateInterface>
ArmHardwareInterface::export_state_interfaces()
{
    std::vector<hardware_interface::StateInterface> state_interfaces;

    for (size_t i = 0; i < motor_ids_.size(); i++) {
        state_interfaces.emplace_back(
            info_.joints[i].name, "position", &hw_positions_[i]);

        state_interfaces.emplace_back(
            info_.joints[i].name, "velocity", &hw_velocities_[i]);

        state_interfaces.emplace_back(
            info_.joints[i].name, "effort", &hw_efforts_[i]);
    }

    return state_interfaces;
}

// -------- COMMAND INTERFACES --------
std::vector<hardware_interface::CommandInterface>
ArmHardwareInterface::export_command_interfaces()
{
    std::vector<hardware_interface::CommandInterface> command_interfaces;

    for (size_t i = 0; i < motor_ids_.size(); i++) {
        command_interfaces.emplace_back(
            info_.joints[i].name, "position", &hw_commands_[i]);
    }

    return command_interfaces;
}



hardware_interface::CallbackReturn ArmHardwareInterface::on_configure(
    const rclcpp_lifecycle::State &)
{
    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "Configuring hardware...");

    transport_ = std::make_unique<SocketCANTransport>(can_interface_);

    motors_.clear();
    id_to_index_.clear();

    for (size_t i = 0; i < motor_ids_.size(); i++) {

        int id = motor_ids_[i];

        motors_.emplace_back(
            std::make_unique<damiao::DamiaoCanDriver>(
                *transport_,
                static_cast<uint16_t>(id),
                0,
                damiao::MappingRange{}
            )
        );

        id_to_index_[id] = i;

        RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                    "Configured motor ID %d at index %zu", id, i);
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}


hardware_interface::CallbackReturn ArmHardwareInterface::on_activate(
    const rclcpp_lifecycle::State &)
{
    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "Activating motors...");

    initialized_ = false;   // 🔥 block write until sync

    for (size_t i = 0; i < motors_.size(); i++) {

        auto & motor = motors_[i];

        // 1. Enable motor
        if (!motor->enable_motor()) {
            RCLCPP_ERROR(rclcpp::get_logger("ArmHardware"),
                         "Failed to enable motor %zu", i);
            return hardware_interface::CallbackReturn::ERROR;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 2. Switch to MIT mode
        motor->switch_mode(damiao::ControlMode::MIT);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 3. Read correct feedback (IMPORTANT)
        damiao::CanFrame frame;
        bool got = false;

        for (int k = 0; k < 15; k++) {

            if (!transport_->recv(frame, 5)) continue;

            auto fb = motor->decode_feedback(frame);
            if (!fb) continue;

            int recv_id = fb->node_low_nibble;

            // 🔥 ONLY accept correct motor frame
            if (recv_id != motor_ids_[i]) continue;

            // ✅ Correct mapping
            hw_positions_[i]  = fb->pos;
            hw_velocities_[i] = fb->vel;
            hw_efforts_[i]    = fb->torque;

            // 🔥 HOLD POSITION (critical)
            hw_commands_[i] = hw_positions_[i];
            hold_positions_[i] = hw_positions_[i];

            got = true;

            RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                        "Motor %zu (ID %d) synced at %.3f",
                        i, motor_ids_[i], hw_positions_[i]);

            break;
        }

        if (!got) {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardware"),
                        "No feedback for motor %zu (ID %d), holding last",
                        i, motor_ids_[i]);

            hw_commands_[i] = hw_positions_[i];
        }
    }

    initialized_ = true;   // 🔥 allow write AFTER sync

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "Motors activated safely (no jump)");

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmHardwareInterface::on_deactivate(
    const rclcpp_lifecycle::State &)
{
    for (auto & motor : motors_) {

        if (!motor->disable_motor()) {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardware"),
                        "Failed to disable motor");
        }
    }

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "All motors deactivated");

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type ArmHardwareInterface::read(
    const rclcpp::Time &,
    const rclcpp::Duration &)
{
    // --------------------------------------------------
    // Safety check
    // --------------------------------------------------
    if (!transport_ || motors_.empty())
    {
        RCLCPP_ERROR(
            rclcpp::get_logger("ArmHardware"),
            "Transport or motors not initialized!");

        return hardware_interface::return_type::ERROR;
    }

    damiao::CanFrame frame;

    bool got_any_data = false;

    // --------------------------------------------------
    // Increment missed cycle counters
    // --------------------------------------------------
    for (size_t i = 0; i < motors_.size(); i++)
    {
        missed_cycles_[i]++;
    }

    // --------------------------------------------------
    // Read ALL available CAN frames
    // --------------------------------------------------
    while (transport_->recv(frame, 0))
    {


        // --------------------------------------------------
        // Extract motor ID
        // --------------------------------------------------
        // int id = frame.arbitration_id & 0x0F;
        int id = frame.arbitration_id - 0x110;
        // --------------------------------------------------
        // Find matching motor index
        // --------------------------------------------------
        auto it = id_to_index_.find(id);

        if (it == id_to_index_.end())
            continue;

        size_t i = it->second;

        if (i >= motors_.size())
            continue;

        // --------------------------------------------------
        // Decode feedback using CORRECT motor
        // --------------------------------------------------
        auto fb = motors_[i]->decode_feedback(frame);

        if (!fb)
            continue;

        // --------------------------------------------------
        // Read REAL encoder values
        // --------------------------------------------------
        double new_pos = fb->pos;
        double new_vel = fb->vel;
        double new_eff = fb->torque;

        // --------------------------------------------------
        // DEBUG encoder values
        // --------------------------------------------------
        double now =
            rclcpp::Clock().now().seconds();

        if ((now - last_debug_print_time_sec_) > 1.0)
        {
            RCLCPP_INFO(
                rclcpp::get_logger("ArmHardware"),
                "Motor %zu (ID %d) "
                "ENC pos=%.6f vel=%.6f torque=%.6f",
                i,
                motor_ids_[i],
                new_pos,
                new_vel,
                new_eff);

            last_debug_print_time_sec_ = now;
        }

        // --------------------------------------------------
        // Reset missed cycles
        // --------------------------------------------------
        missed_cycles_[i] = 0;

        got_any_data = true;

        // --------------------------------------------------
        // LOST -> RECOVERING
        // --------------------------------------------------
        if (motor_states_[i] == MotorState::LOST)
        {
            motor_states_[i] = MotorState::RECOVERING;

            RCLCPP_WARN(
                rclcpp::get_logger("ArmHardware"),
                "Motor %zu (ID %d) RECONNECTED",
                i,
                motor_ids_[i]);
        }

        // --------------------------------------------------
        // RECOVERING
        // --------------------------------------------------
        if (motor_states_[i] == MotorState::RECOVERING)
        {
            // --------------------------------------------------
            // Print REAL recovered encoder
            // --------------------------------------------------
            RCLCPP_WARN(
                rclcpp::get_logger("ArmHardware"),
                "RECOVERY Motor %zu (ID %d) "
                "REAL encoder position %.6f",
                i,
                motor_ids_[i],
                new_pos);

            // --------------------------------------------------
            // Sync state to REAL encoder
            // --------------------------------------------------
            hw_positions_[i]  = new_pos;
            hw_velocities_[i] = 0.0;
            hw_efforts_[i]    = 0.0;

            // --------------------------------------------------
            // CRITICAL:
            // Store HOLD position
            // --------------------------------------------------
            hold_positions_[i] = new_pos;

            // --------------------------------------------------
            // Sync controller command
            // --------------------------------------------------
            // hw_commands_[i] = new_pos;
            // Snapshot current controller command
            recovery_command_snapshot_[i] =
                hw_commands_[i];

            // --------------------------------------------------
            // Debug sync values
            // --------------------------------------------------
            RCLCPP_WARN(
                rclcpp::get_logger("ArmHardware"),
                "SYNC Motor %zu "
                "CMD=%.6f HOLD=%.6f ENC=%.6f",
                i,
                hw_commands_[i],
                hold_positions_[i],
                hw_positions_[i]);

            // --------------------------------------------------
            // Move into HOLDING mode
            // --------------------------------------------------
            motor_states_[i] = MotorState::HOLDING;

            continue;
        }

        // --------------------------------------------------
        // NORMAL / HOLDING
        // --------------------------------------------------
        hw_positions_[i]  = new_pos;
        hw_velocities_[i] = new_vel;
        hw_efforts_[i]    = new_eff;

        // --------------------------------------------------
        // HOLDING debug
        // --------------------------------------------------
    if (motor_states_[i] == MotorState::HOLDING)
    {
        double now =
            rclcpp::Clock().now().seconds();

        if ((now - last_debug_print_time_sec_) > 1.0)
        {
            double err =
                hold_positions_[i] -
                hw_positions_[i];

            RCLCPP_INFO(
                rclcpp::get_logger("ArmHardware"),
                "HOLDING Motor %zu "
                "HOLD=%.6f ENC=%.6f ERR=%.6f",
                i,
                hold_positions_[i],
                hw_positions_[i],
                err);

            last_debug_print_time_sec_ = now;
        }
    }
    }

    // --------------------------------------------------
    // Detect disconnected motors
    // --------------------------------------------------
    for (size_t i = 0; i < motors_.size(); i++)
    {
        // ~0.5 sec timeout @100Hz
        if (missed_cycles_[i] > 50)
        {
            if (motor_states_[i] != MotorState::LOST)
            {
                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "Motor %zu (ID %d) LOST",
                    i,
                    motor_ids_[i]);

                motor_states_[i] =
                    MotorState::LOST;
            }

            // --------------------------------------------------
            // Hold previous state safely
            // --------------------------------------------------
            hw_velocities_[i] = 0.0;
            hw_efforts_[i]    = 0.0;
        }
    }

    // --------------------------------------------------
    // No CAN data
    // --------------------------------------------------
    if (!got_any_data)
    {
        for (size_t i = 0; i < motors_.size(); i++)
        {
            hw_velocities_[i] = 0.0;
            hw_efforts_[i]    = 0.0;
        }
    }

    return hardware_interface::return_type::OK;
}
hardware_interface::return_type ArmHardwareInterface::write(
    const rclcpp::Time &,
    const rclcpp::Duration &)
{
    // --------------------------------------------------
    // Do not send commands before activation sync
    // --------------------------------------------------
    if (!initialized_)
    {
        return hardware_interface::return_type::OK;
    }

    // --------------------------------------------------
    // Safety check
    // --------------------------------------------------
    if (motors_.empty())
    {
        RCLCPP_ERROR(
            rclcpp::get_logger("ArmHardware"),
            "Motors not initialized!");

        return hardware_interface::return_type::ERROR;
    }

    // --------------------------------------------------
    // Send commands
    // --------------------------------------------------
    for (size_t i = 0; i < motors_.size(); i++)
    {
        // --------------------------------------------------
        // Bounds safety
        // --------------------------------------------------
        if (i >= hw_commands_.size() ||
            i >= hw_positions_.size() ||
            i >= kp_.size() ||
            i >= kd_.size())
        {
            RCLCPP_ERROR(
                rclcpp::get_logger("ArmHardware"),
                "Index mismatch at motor %zu",
                i);

            continue;
        }

        // --------------------------------------------------
        // LOST STATE
        // Motor disconnected
        // --------------------------------------------------
        if (motor_states_[i] == MotorState::LOST)
        {
            // --------------------------------------------------
            // Try recovery periodically
            // --------------------------------------------------
            if (missed_cycles_[i] % 100 == 0)
            {
                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "Recovering motor %zu (ID %d)",
                    i,
                    motor_ids_[i]);

                try
                {
                    // Enable motor
                    motors_[i]->enable_motor();

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    // Switch MIT mode
                    motors_[i]->switch_mode(
                        damiao::ControlMode::MIT);

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    // --------------------------------------------------
                    // Move into recovering state
                    // Actual sync happens in read()
                    // --------------------------------------------------
                    motor_states_[i] =
                        MotorState::RECOVERING;
                }
                catch (...)
                {
                    RCLCPP_ERROR(
                        rclcpp::get_logger("ArmHardware"),
                        "Recovery failed motor %zu",
                        i);
                }
            }

            // --------------------------------------------------
            // NEVER send commands while lost
            // --------------------------------------------------
            continue;
        }

        // --------------------------------------------------
        // RECOVERING
        // Waiting for encoder sync in read()
        // --------------------------------------------------
        if (motor_states_[i] == MotorState::RECOVERING)
        {
            continue;
        }



        // --------------------------------------------------
        // HOLDING MODE
        // --------------------------------------------------
        if (motor_states_[i] == MotorState::HOLDING)
        {
            // Hold FIXED recovery position
            float hold_pos =
                static_cast<float>(hold_positions_[i]);

            // --------------------------------------------------
            // Small deadband
            // --------------------------------------------------
            double err =
                std::fabs(
                    hold_positions_[i] -
                    hw_positions_[i]);

            if (err < 0.01)
            {
                hold_pos =
                    static_cast<float>(hw_positions_[i]);
            }

            // --------------------------------------------------
            // Send HOLD command
            // --------------------------------------------------
            try
            {
                motors_[i]->send_mit(
                    hold_pos,
                    0.0f,
                    kp_[i],
                    kd_[i],
                    0.0f);
            }
            catch (...)
            {
                RCLCPP_ERROR(
                    rclcpp::get_logger("ArmHardware"),
                    "Hold failed motor %zu",
                    i);

                motor_states_[i] =
                    MotorState::LOST;
            // Detect REAL new command

                continue;
            }
            // recovery behaviour

            // --------------------------------------------------
            // Detect REAL new command
            // Ignore old buffered trajectory
            // --------------------------------------------------
            double cmd_err =
                std::fabs(
                    hw_commands_[i] -
                    recovery_command_snapshot_[i]);
            // New command received
            // New command received
            if (cmd_err > 0.1)
            {
                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "Motor %zu NEW command received -> NORMAL",
                    i);

                // IMPORTANT:
                // Sync before enabling control
                hw_commands_[i] =
                    hw_positions_[i];

                motor_states_[i] =
                    MotorState::NORMAL;
            }
            continue;
        }
        // --------------------------------------------------
        // NORMAL CONTROL
        // --------------------------------------------------
        float p =
            static_cast<float>(hw_commands_[i]);

        // IMPORTANT:
        // desired velocity should be ZERO
        // NOT measured velocity
        float v = 0.0f;

        float kp = kp_[i];
        float kd = kd_[i];

        float t = 0.0f;

        // --------------------------------------------------
        // Safety clamp
        // --------------------------------------------------
        p = std::clamp(
            p,
            -12.5f,
            12.5f);

        // --------------------------------------------------
        // Send MIT command
        // --------------------------------------------------
        try
        {
            motors_[i]->send_mit(
                p,
                v,
                kp,
                kd,
                t);
        }
        catch (...)
        {
            RCLCPP_ERROR(
                rclcpp::get_logger("ArmHardware"),
                "Send failed for motor %zu",
                i);

            // --------------------------------------------------
            // Move to LOST state
            // --------------------------------------------------
            motor_states_[i] =
                MotorState::LOST;
        }
    }

    return hardware_interface::return_type::OK;
}
} // namespace arm_hardware

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
    arm_hardware::ArmHardwareInterface,
    hardware_interface::SystemInterface
)