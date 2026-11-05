// Publishes the gravity torque each joint needs to hold the robot still.
//
//   /robot_description (std_msgs/String, latched)  URDF the KDL model is built from
//   /joint_states      (sensor_msgs/JointState)    measured joint positions
//   -> /gravity_torque (sensor_msgs/JointState)    name, position (q used), effort (torque)
//
// One chain per entry of tip_links, all from root_link. The torque is computed on every
// /joint_states message, in URDF convention (no hardware direction applied), scaled by
// gravity_scale. joint_cmd adds it to the effort it sends to the PosVelEff controllers.

#include <cmath>
#include <gravity_compensation/gravity_model.hpp>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/string.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

using gravity_compensation::GravityModel;
using sensor_msgs::msg::JointState;

class GravityTorqueNode : public rclcpp::Node {
public:
    GravityTorqueNode() : Node("gravity_torque_node") {
        root_link_ = declare_parameter<std::string>("root_link", "base_link");
        tip_links_ = declare_parameter<std::vector<std::string>>(
            "tip_links", std::vector<std::string>{"arm_link"});
        gravity_vector_ = declare_parameter<std::vector<double>>(
            "gravity_vector", std::vector<double>{0.0, 0.0, -9.81});
        include_beyond_tip_ = declare_parameter<bool>("include_beyond_tip", true);
        gravity_scale_ = declare_parameter<double>("gravity_scale", 1.0);

        param_cb_ = add_on_set_parameters_callback([this](const auto& params) {
            rcl_interfaces::msg::SetParametersResult result;
            result.successful = true;
            for (const auto& p : params) {
                if (p.get_name() == "gravity_scale") {
                    gravity_scale_ = p.as_double();
                    RCLCPP_INFO(get_logger(), "[param] gravity_scale -> %.3f", gravity_scale_);
                } else if (p.get_name() != "use_sim_time") {
                    result.successful = false;
                    result.reason = p.get_name() + " is read-only after start";
                }
            }
            return result;
        });

        // robot_state_publisher latches the description.
        description_sub_ = create_subscription<std_msgs::msg::String>(
            "/robot_description", rclcpp::QoS(1).transient_local().reliable(),
            [this](const std_msgs::msg::String& msg) { OnRobotDescription(msg); });

        joint_state_sub_ = create_subscription<JointState>(
            "/joint_states", 10, [this](const JointState& msg) { OnJointState(msg); });

        pub_ = create_publisher<JointState>("/gravity_torque", 10);

        RCLCPP_INFO(get_logger(), "Waiting for /robot_description");
    }

private:
    void OnRobotDescription(const std_msgs::msg::String& msg) {
        std::vector<std::unique_ptr<GravityModel>> models;
        try {
            for (const auto& tip : tip_links_) {
                auto model = std::make_unique<GravityModel>(msg.data, root_link_, tip,
                                                            gravity_vector_, include_beyond_tip_);
                model->Init();
                models.push_back(std::move(model));
            }
        } catch (const std::exception& e) {
            RCLCPP_ERROR(get_logger(), "Cannot build gravity model: %s", e.what());
            return;
        }

        models_ = std::move(models);
        joint_names_.clear();
        for (const auto& model : models_) {
            joint_names_.push_back(model->JointNames());
            std::string names;
            for (const auto& n : joint_names_.back()) names += " " + n;
            RCLCPP_INFO(get_logger(), "Gravity model ready: %s -> %s, joints:%s",
                        model->RootLink().c_str(), model->TipLink().c_str(), names.c_str());
        }
    }

    void OnJointState(const JointState& msg) {
        if (models_.empty()) return;

        std::unordered_map<std::string, double> positions;
        for (size_t i = 0; i < msg.name.size() && i < msg.position.size(); ++i) {
            positions[msg.name[i]] = msg.position[i];
        }

        JointState out;
        out.header.stamp = msg.header.stamp;

        for (size_t m = 0; m < models_.size(); ++m) {
            const auto& names = joint_names_[m];
            q_.assign(names.size(), 0.0);
            bool valid = true;
            for (size_t i = 0; i < names.size(); ++i) {
                const auto it = positions.find(names[i]);
                if (it == positions.end() || !std::isfinite(it->second)) {
                    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000,
                                         "No valid /joint_states position for %s; skipping chain "
                                         "-> %s",
                                         names[i].c_str(), models_[m]->TipLink().c_str());
                    valid = false;
                    break;
                }
                q_[i] = it->second;
            }
            if (!valid) continue;

            models_[m]->ComputeGravity(q_, tau_);
            for (size_t i = 0; i < names.size(); ++i) {
                out.name.push_back(names[i]);
                out.position.push_back(q_[i]);
                out.effort.push_back(gravity_scale_ * tau_[i]);
            }
        }

        if (!out.name.empty()) pub_->publish(out);
    }

    std::string root_link_;
    std::vector<std::string> tip_links_;
    std::vector<double> gravity_vector_;
    bool include_beyond_tip_;
    double gravity_scale_;

    std::vector<std::unique_ptr<GravityModel>> models_;
    std::vector<std::vector<std::string>> joint_names_;
    std::vector<double> q_, tau_;

    OnSetParametersCallbackHandle::SharedPtr param_cb_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr description_sub_;
    rclcpp::Subscription<JointState>::SharedPtr joint_state_sub_;
    rclcpp::Publisher<JointState>::SharedPtr pub_;
};

}  // namespace

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GravityTorqueNode>());
    rclcpp::shutdown();
    return 0;
}
