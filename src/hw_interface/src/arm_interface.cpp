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

std::string decode_error(int err)
{
    switch (err)
    {
        case 0:
            return "Disabled";

        case 1:
            return "Enabled";

        case 8:
            return "Overvoltage";

        case 9:
            return "Undervoltage";

        case 10:
            return "Overcurrent";

        case 11:
            return "MOS Overtemperature";

        case 12:
            return "Motor Coil Overtemperature";

        case 13:
            return "Communication Lost";

        case 14:
            return "Overload";

        default:
            return "Unknown";
    }
}
hardware_interface::CallbackReturn
ArmHardwareInterface::on_init(
    const hardware_interface::HardwareComponentInterfaceParams & params)
{
    const auto & info = params.hardware_info;

    // --------------------------------------------------
    // Base class init
    // --------------------------------------------------
    if (SystemInterface::on_init(params) !=
        hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    info_ = info;
    arm_name_ = info_.name;

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
    joint_is_prismatic_.resize(n, false);
    for (size_t i = 0; i < info_.joints.size(); i++)
    {
        const auto & joint = info_.joints[i];

        auto type_it =
            joint.parameters.find("type");

        if (
            type_it != joint.parameters.end()
        )
        {
            if (
                type_it->second == "prismatic"
            )
            {
                joint_is_prismatic_[i] = true;

                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "Joint %s is PRISMATIC",
                    joint.name.c_str());
            }
            else
            {
                RCLCPP_INFO(
                    rclcpp::get_logger("ArmHardware"),
                    "Joint %s is REVOLUTE",
                    joint.name.c_str());
            }
        }
    }
    motor_errors_.assign(n, 0);
    motor_mos_temp_.assign(n, 0.0f);
    motor_rotor_temp_.assign(n, 0.0f);

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
    // Parse direction
    // --------------------------------------------------
    if (info_.hardware_parameters.count("direction"))
    {
        direction_ =
            parse_csv<int>(
                info_.hardware_parameters.at("direction"));
    }
    else
    {
        direction_ =
            std::vector<int>(n, 1);
    }

    if (direction_.size() != n)
    {
        RCLCPP_ERROR(
            rclcpp::get_logger("ArmHardware"),
            "Direction size mismatch");

        return hardware_interface::CallbackReturn::ERROR;
    }

    // --------------------------------------------------
    // Parse per-joint control mode (mit / posvel)
    // --------------------------------------------------
    if (info_.hardware_parameters.count("control_modes"))
    {
        std::stringstream ss(
            info_.hardware_parameters.at("control_modes"));
        std::string token;

        while (std::getline(ss, token, ','))
        {
            if (token == "mit")
            {
                control_modes_.push_back(
                    HardwareControlMode::MIT);
            }
            else if (token == "posvel")
            {
                control_modes_.push_back(
                    HardwareControlMode::POSVEL);
            }
            else
            {
                RCLCPP_ERROR(
                    rclcpp::get_logger("ArmHardware"),
                    "Unknown control mode '%s'",
                    token.c_str());

                return hardware_interface::CallbackReturn::ERROR;
            }
        }

        if (control_modes_.size() != n)
        {
            RCLCPP_ERROR(
                rclcpp::get_logger("ArmHardware"),
                "control_modes size mismatch: expected %zu got %zu",
                n,
                control_modes_.size());

            return hardware_interface::CallbackReturn::ERROR;
        }
    }
    else
    {
        control_modes_ =
            std::vector<HardwareControlMode>(
                n, HardwareControlMode::MIT);
    }

    // --------------------------------------------------
    // Parse MIT mapping ranges (p_max / v_max / t_max)
    //
    // These MUST equal the motor's PMAX/VMAX/TMAX registers
    // (RID 21/22/23), otherwise every MIT command and feedback
    // value is scaled wrong (e.g. DM4340 TMAX=28 vs default 10
    // -> 2.8x torque). One value applies to all motors.
    // --------------------------------------------------
    auto parse_range =
        [&](const std::string & key, float def, std::vector<float> & out) -> bool
    {
        if (!info_.hardware_parameters.count(key))
        {
            out.assign(n, def);
            return true;
        }
        out = parse_csv<float>(info_.hardware_parameters.at(key));
        if (out.size() == 1)
            out.assign(n, out[0]);
        if (out.size() != n)
        {
            RCLCPP_ERROR(
                rclcpp::get_logger("ArmHardware"),
                "%s size mismatch: expected 1 or %zu got %zu",
                key.c_str(), n, out.size());
            return false;
        }
        for (float v : out)
        {
            if (!(v > 0.0f))
            {
                RCLCPP_ERROR(
                    rclcpp::get_logger("ArmHardware"),
                    "%s must be > 0", key.c_str());
                return false;
            }
        }
        return true;
    };

    if (!parse_range("p_max", 12.5f, p_max_) ||
        !parse_range("v_max", 30.0f, v_max_) ||
        !parse_range("t_max", 10.0f, t_max_))
    {
        return hardware_interface::CallbackReturn::ERROR;
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
    hw_velocity_commands_.assign(n, 0.0);
    hw_torque_commands_.assign(n, 0.0);

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
                // --------------------------------------------------
        // Recovery service node
        // --------------------------------------------------
        recovery_node_ =
            std::make_shared<rclcpp::Node>(
                arm_name_ + "_recovery_service_node");

        recovery_service_ =
            recovery_node_->create_service<std_srvs::srv::Trigger>(
                "/" + arm_name_ + "/arm_recover",
                std::bind(
                    &ArmHardwareInterface::recovery_service_callback,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));
        // --------------------------------------------------
        // Motor status publisher
        // --------------------------------------------------
        motor_status_pub_ =
            recovery_node_->create_publisher<
                hw_interface::msg::MotorStatusArray>(
                    "/" + arm_name_ + "/motor_status",
                    10);
        joint_state_pub_ =
            recovery_node_->create_publisher<
                sensor_msgs::msg::JointState>(
                    "/" + arm_name_ + "_ordered_joint_states",
                    10);

        write_debug_pub_ =
            recovery_node_->create_publisher<
                sensor_msgs::msg::JointState>(
                    "/" + arm_name_ + "_hw_write_cmd",
                    10);

        // Spin node in background
        std::thread([this]()
        {
            rclcpp::spin(recovery_node_);
        }).detach();

    return hardware_interface::CallbackReturn::SUCCESS;
}
void ArmHardwareInterface::recovery_service_callback(
    const std::shared_ptr<std_srvs::srv::Trigger::Request>,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response)
{
    recovery_requested_ = true;

    response->success = true;
    response->message = "Recovery requested";

    RCLCPP_WARN(
        rclcpp::get_logger("ArmHardware"),
        "Manual recovery requested");
}

void ArmHardwareInterface::set_motor_gain_callback(
    const std::shared_ptr<
        hw_interface::srv::SetMotorGains::Request> request,
    std::shared_ptr<
        hw_interface::srv::SetMotorGains::Response> response)
{
    // =========================================
    // SERVICE CALLED DEBUG
    // =========================================
    RCLCPP_WARN(
        recovery_node_->get_logger(),
        "[%s] Gain service called",
        arm_name_.c_str());

    size_t n = request->motor_ids.size();

    if (request->kp.size() != n ||
        request->kd.size() != n)
    {
        response->success = false;
        response->message = "Array size mismatch";
        return;
    }

    for (size_t i = 0; i < n; i++)
    {
        int id = request->motor_ids[i];

        // =========================================
        // RECEIVED ID DEBUG
        // =========================================
        RCLCPP_INFO(
            recovery_node_->get_logger(),
            "[%s] Received motor ID %d",
            arm_name_.c_str(),
            id);

        auto it = id_to_index_.find(id);

        if (it == id_to_index_.end())
        {
            RCLCPP_ERROR(
                recovery_node_->get_logger(),
                "[%s] Motor ID %d NOT FOUND",
                arm_name_.c_str(),
                id);

            continue;
        }

        size_t idx = it->second;

        kp_[idx] = request->kp[i];
        kd_[idx] = request->kd[i];

        RCLCPP_INFO(
            recovery_node_->get_logger(),
            "[%s] Motor %d -> kp=%.2f kd=%.3f",
            arm_name_.c_str(),
            id,
            kp_[idx],
            kd_[idx]);
    }

    response->success = true;
    response->message = "Motor gains updated";
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

    for (size_t i = 0; i < motor_ids_.size(); i++)
    {
        command_interfaces.emplace_back(
            info_.joints[i].name,
            "position",
            &hw_commands_[i]);

        command_interfaces.emplace_back(
            info_.joints[i].name,
            "velocity",
            &hw_velocity_commands_[i]);

        command_interfaces.emplace_back(
            info_.joints[i].name,
            "effort",
            &hw_torque_commands_[i]);
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

        damiao::MappingRange range;
        range.p_min = -p_max_[i];
        range.p_max = p_max_[i];
        range.v_min = -v_max_[i];
        range.v_max = v_max_[i];
        range.t_min = -t_max_[i];
        range.t_max = t_max_[i];

        motors_.emplace_back(
            std::make_unique<damiao::DamiaoCanDriver>(
                *transport_,
                static_cast<uint16_t>(id),
                0,
                range
            )
        );

        id_to_index_[id] = i;

        RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                    "Motor ID %d mapping: p=+-%.2f v=+-%.2f t=+-%.2f",
                    id, p_max_[i], v_max_[i], t_max_[i]);

        RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                    "Configured motor ID %d at index %zu", id, i);
    }
    gain_service_ =
        recovery_node_->create_service<
            hw_interface::srv::SetMotorGains>(
            "/" + arm_name_ + "/set_motor_gains",
            std::bind(
                &ArmHardwareInterface::set_motor_gain_callback,
                this,
                std::placeholders::_1,
                std::placeholders::_2));

    // --------------------------------------------------
    // Force-disable on rclcpp shutdown (Ctrl+C, SIGTERM, ...).
    //
    // on_deactivate()/on_shutdown() only run if controller_manager
    // drives the hardware component through a full lifecycle
    // transition, which is not guaranteed on process termination.
    // rclcpp::on_shutdown() fires whenever the ROS context shuts
    // down, regardless of whether that lifecycle transition ever
    // happens, so it is the last safety net before the process
    // exits. It cannot help against SIGKILL / crash / power loss —
    // those still require a hardware E-stop.
    // --------------------------------------------------
    if (!shutdown_hook_registered_)
    {
        rclcpp::on_shutdown(
            [this]()
            {
                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "[%s] rclcpp shutdown detected, "
                    "force-disabling motors",
                    arm_name_.c_str());

                disable_all_motors();
            });

        shutdown_hook_registered_ = true;
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

        // 1. Motor must be DISABLED to accept a mode register write;
        //    the Damiao firmware silently ignores switch_mode() while
        //    the motor is enabled, which is why commands in the new
        //    mode were being sent but never acted on.
        motor->disable_motor();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 2. Switch to the configured control mode, then persist it
        motor->switch_mode(
            control_modes_[i] == HardwareControlMode::MIT
                ? damiao::ControlMode::MIT
                : damiao::ControlMode::POSITION_VELOCITY);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        motor->store_parameters();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 3. Enable motor
        if (!motor->enable_motor()) {
            RCLCPP_ERROR(rclcpp::get_logger("ArmHardware"),
                         "Failed to enable motor %zu", i);
            return hardware_interface::CallbackReturn::ERROR;
        }

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

            double pos = fb->pos;
            double vel = fb->vel;

            if (joint_is_prismatic_[i])
            {
                pos = motor_rad_to_linear(pos);
                vel = motor_rad_to_linear(vel);
            }

            // ✅ Correct mapping
            hw_positions_[i]  = pos;
            hw_velocities_[i] = vel;
            hw_efforts_[i]    = fb->torque;

            // 🔥 HOLD POSITION (critical)
            hw_commands_[i]   = pos;
            hold_positions_[i]= pos;



            got = true;

            RCLCPP_INFO(
                rclcpp::get_logger("ArmHardware"),
                "[%s] Motor %zu (ID %d) synced at %.3f",
                arm_name_.c_str(),
                i,
                motor_ids_[i],
                hw_positions_[i]);

            break;
        }

        if (!got) {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardware"),
                        "No feedback for motor %zu (ID %d), holding last",
                        i, motor_ids_[i]);

            hw_commands_[i] = hw_positions_[i];
            hold_positions_[i] = hw_positions_[i];
        }
    }

    initialized_ = true;   // 🔥 allow write AFTER sync

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "Motors activated safely (no jump)");

    return hardware_interface::CallbackReturn::SUCCESS;
}

void ArmHardwareInterface::disable_all_motors()
{
    for (auto & motor : motors_) {

        if (!motor->disable_motor()) {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardware"),
                        "Failed to disable motor");
        }
    }
}

hardware_interface::CallbackReturn ArmHardwareInterface::on_deactivate(
    const rclcpp_lifecycle::State &)
{
    disable_all_motors();

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "All motors deactivated");

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmHardwareInterface::on_shutdown(
    const rclcpp_lifecycle::State &)
{
    // Reached on process termination (e.g. Ctrl+C on the launch file):
    // the lifecycle state machine transitions ACTIVE -> FINALIZED
    // directly here, skipping on_deactivate, so motors must be
    // disabled again on this path.
    disable_all_motors();

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "All motors disabled on shutdown");

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmHardwareInterface::on_error(
    const rclcpp_lifecycle::State &)
{
    disable_all_motors();

    RCLCPP_INFO(rclcpp::get_logger("ArmHardware"),
                "All motors disabled after error");

    return hardware_interface::CallbackReturn::SUCCESS;
}

double ArmHardwareInterface::linear_to_motor_rad(
    double meter)
{
    return (
        meter /
        gripper_joint_max_
    ) *
    gripper_motor_max_rad_;
}

double ArmHardwareInterface::motor_rad_to_linear(
    double rad)
{
    return (
        gripper_joint_max_ *
        (
            rad /
            gripper_motor_max_rad_
        )
    );
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
        int id = frame.arbitration_id & 0x0F;

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
        // double new_pos = fb->pos;
        // double new_vel = fb->vel;
        // double new_eff = fb->torque;
        double dir =
            static_cast<double>(direction_[i]);

        double new_pos =
            fb->pos * dir;

        double new_vel =
            fb->vel * dir;

        double new_eff =
            fb->torque * dir;
        // --------------------------------------------------
        // Parse motor status
        // --------------------------------------------------
        uint8_t raw0 =
            frame.data[0];

        motor_errors_[i] =
            (raw0 >> 4) & 0x0F;

        motor_mos_temp_[i] =
            static_cast<float>(frame.data[6]);

        motor_rotor_temp_[i] =
            static_cast<float>(frame.data[7]);

        // --------------------------------------------------
        // DEBUG encoder values
        // --------------------------------------------------
        // double now =
        //     rclcpp::Clock().now().seconds();

        // if ((now - last_debug_print_time_sec_) > 1.0)
        // {
        //     RCLCPP_INFO(
        //         rclcpp::get_logger("ArmHardware"),
        //         "Motor %zu (ID %d) "
        //         "ENC pos=%.6f vel=%.6f torque=%.6f",
        //         i,
        //         motor_ids_[i],
        //         new_pos,
        //         new_vel,
        //         new_eff);

        //     last_debug_print_time_sec_ = now;
        // }

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
            if (joint_is_prismatic_[i])
            {
                hw_commands_[i] =
                    motor_rad_to_linear(new_pos);
            }
            else
            {
                hw_commands_[i] =
                    new_pos;
            }
            if (joint_is_prismatic_[i])
            {
                hw_positions_[i] =
                    motor_rad_to_linear(new_pos);

                hold_positions_[i] =
                    motor_rad_to_linear(new_pos);
            }
            else
            {
                hw_positions_[i] =
                    new_pos;

                hold_positions_[i] =
                    new_pos;
            }

            hw_velocities_[i] = 0.0;
            hw_efforts_[i]    = 0.0;

            // --------------------------------------------------
            // CRITICAL:
            // Store HOLD position
            // --------------------------------------------------

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
        if (joint_is_prismatic_[i])
        {
            hw_positions_[i] =
                motor_rad_to_linear(new_pos);

            hw_velocities_[i] =
                motor_rad_to_linear(new_vel);
        }
        else
        {
            hw_positions_[i] =
                new_pos;

            hw_velocities_[i] =
                new_vel;
        }

        hw_efforts_[i] = new_eff;

        // --------------------------------------------------
        // HOLDING debug
        // --------------------------------------------------
    // if (motor_states_[i] == MotorState::HOLDING)
    // {
    //     double now =
    //         rclcpp::Clock().now().seconds();

    //     if ((now - last_debug_print_time_sec_) > 1.0)
    //     {
    //         double err =
    //             hold_positions_[i] -
    //             hw_positions_[i];

    //         RCLCPP_INFO(
    //             rclcpp::get_logger("ArmHardware"),
    //             "HOLDING Motor %zu "
    //             "HOLD=%.6f ENC=%.6f ERR=%.6f",
    //             i,
    //             hold_positions_[i],
    //             hw_positions_[i],
    //             err);

    //         last_debug_print_time_sec_ = now;
    //     }
    // }
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
                "[%s] Motor %zu (ID %d) LOST",
                arm_name_.c_str(),
                i,
                motor_ids_[i]);

                motor_states_[i] =
                    MotorState::LOST;
            }

            // --------------------------------------------------
            // Communication lost status
            // --------------------------------------------------
            motor_errors_[i] = 13;
            motor_mos_temp_[i]   = -1.0f;
            motor_rotor_temp_[i] = -1.0f;

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
    // --------------------------------------------------
    // Publish motor status
    // --------------------------------------------------
    hw_interface::msg::MotorStatusArray msg;

    for (size_t i = 0; i < motors_.size(); i++)
    {
        hw_interface::msg::MotorStatus status;
        status.arm_name = arm_name_;

        status.id =
            motor_ids_[i];

        status.error =
            motor_errors_[i];

        status.error_name =
            decode_error(
                motor_errors_[i]);

        status.mos_temp =
            motor_mos_temp_[i];

        status.rotor_temp =
            motor_rotor_temp_[i];

        msg.motors.push_back(status);
    }

    motor_status_pub_->publish(msg);


    sensor_msgs::msg::JointState js;

        js.header.stamp =
            recovery_node_->now();

        js.header.frame_id =
            "base_link";

    for (size_t i = 0; i < motor_ids_.size(); i++)
    {
        js.name.push_back(
            info_.joints[i].name);

        js.position.push_back(
            hw_positions_[i]);

        js.velocity.push_back(
            hw_velocities_[i]);

        js.effort.push_back(
            hw_efforts_[i]);
    }
    joint_state_pub_->publish(js);

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
    // Debug tap: snapshot the raw incoming command_interfaces_ values
    // before any per-motor state-machine branch or direction_/prismatic
    // transform touches them, for comparing against posveleff_controllers'
    // debug_cmd_pub_ to measure the update()->write() hand-off delay.
    // --------------------------------------------------
    sensor_msgs::msg::JointState write_dbg;
    write_dbg.header.stamp = recovery_node_->now();

    for (size_t i = 0; i < motors_.size(); i++)
    {
        write_dbg.name.push_back(info_.joints[i].name);
        write_dbg.position.push_back(hw_commands_[i]);
        write_dbg.velocity.push_back(hw_velocity_commands_[i]);
        write_dbg.effort.push_back(hw_torque_commands_[i]);
    }

    write_debug_pub_->publish(write_dbg);

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
            if (recovery_requested_)
            {
                RCLCPP_WARN(
                    rclcpp::get_logger("ArmHardware"),
                    "Recovering motor %zu (ID %d)",
                    i,
                    motor_ids_[i]);

                try
                {
                    // Mode must be switched while DISABLED, same as on_activate()
                    motors_[i]->disable_motor();

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    // Switch back to the configured control mode
                    motors_[i]->switch_mode(
                        control_modes_[i] == HardwareControlMode::MIT
                            ? damiao::ControlMode::MIT
                            : damiao::ControlMode::POSITION_VELOCITY);

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    motors_[i]->store_parameters();

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    // Enable motor
                    motors_[i]->enable_motor();

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(200));

                    // --------------------------------------------------
                    // Move into recovering state
                    // Actual sync happens in read()
                    // --------------------------------------------------
                    motor_states_[i] =
                        MotorState::RECOVERING;
                        recovery_requested_ = false;
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
            float hold_pos;

            if (joint_is_prismatic_[i])
            {
                hold_pos = static_cast<float>(
                    linear_to_motor_rad(
                        hold_positions_[i]
                    ) * direction_[i]);
            }
            else
            {
                hold_pos = static_cast<float>(
                    hold_positions_[i] *
                    direction_[i]);
            }
            // --------------------------------------------------
            // Small deadband
            // --------------------------------------------------
            double err =
                std::fabs(
                    hold_positions_[i] -
                    hw_positions_[i]);

            if (err < 0.01)
            {
                if (joint_is_prismatic_[i])
                {
                    hold_pos = static_cast<float>(
                        linear_to_motor_rad(
                            hw_positions_[i]
                        ) * direction_[i]);
                }
                else
                {
                    hold_pos = static_cast<float>(
                        hw_positions_[i] *
                        direction_[i]);
                }
            }
            // --------------------------------------------------
            // Send HOLD command
            // --------------------------------------------------
            try
            {
                if (control_modes_[i] == HardwareControlMode::MIT)
                {
                    motors_[i]->send_mit(
                        hold_pos,
                        0.0f,
                        kp_[i],
                        kd_[i],
                        0.0f);
                }
                else
                {
                    motors_[i]->send_posvel(hold_pos, 0.0f);
                }
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
            //
            // Compared in motor-frame units (like p above), not raw
            // joint units: a fixed 0.1 threshold in joint units is
            // fine for a several-radian arm joint but is more than
            // double the gripper's entire 0.044 m range, making this
            // check permanently unsatisfiable for it -- once a transient
            // CAN hiccup ever dropped the gripper into HOLDING, it could
            // never see cmd_err > 0.1 and would stay stuck there,
            // ignoring the live command and resisting any movement,
            // until the hardware interface was restarted.
            // --------------------------------------------------
            double cmd_now = hw_commands_[i];
            double cmd_prev = recovery_command_snapshot_[i];

            if (joint_is_prismatic_[i])
            {
                cmd_now = linear_to_motor_rad(cmd_now);
                cmd_prev = linear_to_motor_rad(cmd_prev);
            }

            double cmd_err =
                std::fabs(cmd_now - cmd_prev);
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
        // float p =
        //     static_cast<float>(hw_commands_[i]);
        // float p =
        //     static_cast<float>(
        //         hw_commands_[i] *
        //         direction_[i]);
        float p;

        if (joint_is_prismatic_[i])
        {
            p = static_cast<float>(
                linear_to_motor_rad(
                    hw_commands_[i]
                ) *
                direction_[i]);
        }
        else
        {
            p = static_cast<float>(
                hw_commands_[i] *
                direction_[i]);
        }
        // IMPORTANT:
        // desired velocity should be ZERO
        // NOT measured velocity
        //
        // Must mirror the position branch above exactly: without
        // linear_to_motor_rad() the gripper's velocity feedforward is
        // ~41x too small (motor_rad vs. joint meters), and without
        // direction_[i] it's unsigned for every joint whose motor is
        // mounted reversed (direction=-1) -- both silently wrong before
        // this fix, for every joint type, not just the gripper.
        float v;

        if (joint_is_prismatic_[i])
        {
            v = static_cast<float>(
                linear_to_motor_rad(
                    hw_velocity_commands_[i]
                ) *
                direction_[i]);
        }
        else
        {
            v = static_cast<float>(
                hw_velocity_commands_[i] *
                direction_[i]);
        }

        float t =
            static_cast<float>(
                hw_torque_commands_[i]);
        float kp = kp_[i];
        float kd = kd_[i];


        // --------------------------------------------------
        // Safety clamp
        // --------------------------------------------------
        p = std::clamp(
            p,
            -12.5f,
            12.5f);

        // --------------------------------------------------
        // Send command in the configured control mode
        // --------------------------------------------------
        try
        {
            if (control_modes_[i] == HardwareControlMode::MIT)
            {
                motors_[i]->send_mit(
                    p,
                    v,
                    kp,
                    kd,
                    t);
            }
            else
            {
                motors_[i]->send_posvel(p, v);
            }
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