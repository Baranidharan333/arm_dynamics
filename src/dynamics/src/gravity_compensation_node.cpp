// gravity_compensation_node: model-based feedforward torque for each joint.
//
//   /robot_description (std_msgs/String, latched)            URDF the KDL models are built from
//   /joint_states      (sensor_msgs/JointState)              measured q
//   /joint_desired     (trajectory_msgs/JointTrajectory)     desired q_d, dq_d, ddq_d (first
//                                                            point), one or more joints per msg
//   -> /feedforward_torque (sensor_msgs/JointState)          effort = tau_ff
//   -> /gravity_torque     (sensor_msgs/JointState)          effort = G(q) term only
//
//   tau_ff = feedforward_scale * ramp(t) * ( inertia_scale  * M(q_d) ddq_d
//                                          + coriolis_scale * C(q_d, dq_d) dq_d
//                                          + gravity_scale  * G(q) )
//
// ramp(t) fades the torque in once at start-up, so the arm is not kicked when the node is
// launched: it goes smoothly from 0 to 1 over ramp_duration seconds, counted from the first
// torque published, and stays at 1 after that (a new /robot_description does not restart it).
// ramp_duration = 0 disables it.
//
// G uses the measured q (holds the arm where it really is); M and C use the desired trajectory,
// so no noisy measured velocity is fed back through C. A joint with no fresh /joint_desired
// (older than desired_timeout) is taken as q_d = q, dq_d = ddq_d = 0, which leaves only G.
//
// One chain per entry of tip_links, all from root_link. Computed on every /joint_states
// message, in URDF convention (no hardware direction applied). joint_cmd adds the effort to
// what it sends to the PosVelEff controllers.

#include <algorithm>
#include <cmath>
#include <optional>
#include <dynamics/coriolis_model.hpp>
#include <dynamics/gravity_model.hpp>
#include <dynamics/inertia_model.hpp>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/string.hpp>
#include <string>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <unordered_map>
#include <vector>

namespace {

using dynamics::CoriolisModel;
using dynamics::GravityModel;
using dynamics::InertiaModel;
using sensor_msgs::msg::JointState;
using trajectory_msgs::msg::JointTrajectory;

struct Desired {
    double q = 0.0;
    double dq = 0.0;
    double ddq = 0.0;
    rclcpp::Time stamp;
};

struct ChainModels {
    std::unique_ptr<GravityModel> gravity;
    std::unique_ptr<InertiaModel> inertia;
    std::unique_ptr<CoriolisModel> coriolis;
};

class GravityCompensationNode : public rclcpp::Node {
public:
    GravityCompensationNode() : Node("gravity_compensation_node") {
        root_link_ = declare_parameter<std::string>("root_link", "base_link");
        tip_links_ = declare_parameter<std::vector<std::string>>(
            "tip_links", std::vector<std::string>{"arm_link"});
        gravity_vector_ = declare_parameter<std::vector<double>>(
            "gravity_vector", std::vector<double>{0.0, 0.0, -9.81});
        include_beyond_tip_ = declare_parameter<bool>("include_beyond_tip", true);
        gravity_scale_ = declare_parameter<double>("gravity_scale", 1.0);
        inertia_scale_ = declare_parameter<double>("inertia_scale", 1.0);
        coriolis_scale_ = declare_parameter<double>("coriolis_scale", 1.0);
        desired_timeout_ = declare_parameter<double>("desired_timeout", 0.1);
        feedforward_scale_ = declare_parameter<double>("feedforward_scale", 1.0);
        ramp_duration_ = declare_parameter<double>("ramp_duration", 3.0);

        param_cb_ = add_on_set_parameters_callback([this](const auto& params) {
            rcl_interfaces::msg::SetParametersResult result;
            result.successful = true;
            for (const auto& p : params) {
                const auto& name = p.get_name();
                if (name == "gravity_scale") {
                    gravity_scale_ = p.as_double();
                } else if (name == "inertia_scale") {
                    inertia_scale_ = p.as_double();
                } else if (name == "coriolis_scale") {
                    coriolis_scale_ = p.as_double();
                } else if (name == "desired_timeout") {
                    desired_timeout_ = p.as_double();
                } else if (name == "feedforward_scale") {
                    feedforward_scale_ = p.as_double();
                } else if (name == "ramp_duration") {
                    ramp_duration_ = p.as_double();
                } else if (name != "use_sim_time") {
                    result.successful = false;
                    result.reason = name + " is read-only after start";
                    continue;
                }
                if (name != "use_sim_time") {
                    RCLCPP_INFO(get_logger(), "[param] %s -> %.3f", name.c_str(),
                                p.as_double());
                }
            }
            return result;
        });

        // robot_state_publisher latches the description.
        description_sub_ = create_subscription<std_msgs::msg::String>(
            "/robot_description", rclcpp::QoS(1).transient_local().reliable(),
            [this](const std_msgs::msg::String& msg) { OnRobotDescription(msg); });

        desired_sub_ = create_subscription<JointTrajectory>(
            "/joint_desired", 10, [this](const JointTrajectory& msg) { OnDesired(msg); });

        joint_state_sub_ = create_subscription<JointState>(
            "/joint_states", 10, [this](const JointState& msg) { OnJointState(msg); });

        ff_pub_ = create_publisher<JointState>("/feedforward_torque", 10);
        gravity_pub_ = create_publisher<JointState>("/gravity_torque", 10);

        RCLCPP_INFO(get_logger(), "Waiting for /robot_description");
    }

private:
    void OnRobotDescription(const std_msgs::msg::String& msg) {
        std::vector<ChainModels> models;
        try {
            for (const auto& tip : tip_links_) {
                ChainModels m;
                m.gravity = std::make_unique<GravityModel>(msg.data, root_link_, tip,
                                                           gravity_vector_, include_beyond_tip_);
                m.inertia = std::make_unique<InertiaModel>(msg.data, root_link_, tip,
                                                           include_beyond_tip_);
                m.coriolis = std::make_unique<CoriolisModel>(msg.data, root_link_, tip,
                                                             include_beyond_tip_);
                m.gravity->Init();
                m.inertia->Init();
                m.coriolis->Init();
                models.push_back(std::move(m));
            }
        } catch (const std::exception& e) {
            RCLCPP_ERROR(get_logger(), "Cannot build dynamics model: %s", e.what());
            return;
        }

        models_ = std::move(models);
        for (const auto& m : models_) {
            std::string names;
            for (const auto& n : m.gravity->JointNames()) names += " " + n;
            RCLCPP_INFO(get_logger(), "Dynamics model ready: %s -> %s, joints:%s",
                        m.gravity->RootLink().c_str(), m.gravity->TipLink().c_str(),
                        names.c_str());
        }
    }

    void OnDesired(const JointTrajectory& msg) {
        if (msg.points.empty()) return;
        const auto& p = msg.points.front();
        const auto now = get_clock()->now();
        for (size_t i = 0; i < msg.joint_names.size(); ++i) {
            if (i >= p.positions.size() || !std::isfinite(p.positions[i])) continue;
            Desired d;
            d.q = p.positions[i];
            d.dq = i < p.velocities.size() && std::isfinite(p.velocities[i]) ? p.velocities[i]
                                                                               : 0.0;
            d.ddq = i < p.accelerations.size() && std::isfinite(p.accelerations[i])
                        ? p.accelerations[i]
                        : 0.0;
            d.stamp = now;
            desired_[msg.joint_names[i]] = d;
        }
    }

    void OnJointState(const JointState& msg) {
        if (models_.empty()) return;

        positions_.clear();
        for (size_t i = 0; i < msg.name.size() && i < msg.position.size(); ++i) {
            positions_[msg.name[i]] = msg.position[i];
        }
        const auto now = get_clock()->now();

        JointState ff, grav;
        ff.header.stamp = msg.header.stamp;
        grav.header.stamp = msg.header.stamp;

        for (auto& m : models_) {
            const auto& names = m.gravity->JointNames();
            const auto n = names.size();
            q_.assign(n, 0.0);
            qd_.assign(n, 0.0);
            dqd_.assign(n, 0.0);
            ddqd_.assign(n, 0.0);

            bool valid = true;
            for (size_t i = 0; i < n; ++i) {
                const auto it = positions_.find(names[i]);
                if (it == positions_.end() || !std::isfinite(it->second)) {
                    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000,
                                         "No valid /joint_states position for %s; skipping chain "
                                         "-> %s",
                                         names[i].c_str(), m.gravity->TipLink().c_str());
                    valid = false;
                    break;
                }
                q_[i] = it->second;
                qd_[i] = q_[i];

                const auto d = desired_.find(names[i]);
                if (d != desired_.end() &&
                    (now - d->second.stamp).seconds() <= desired_timeout_) {
                    qd_[i] = d->second.q;
                    dqd_[i] = d->second.dq;
                    ddqd_[i] = d->second.ddq;
                }
            }
            if (!valid) continue;

            m.gravity->ComputeGravity(q_, tau_g_);
            m.inertia->ComputeInertialTorque(qd_, ddqd_, tau_m_);
            m.coriolis->ComputeCoriolis(qd_, dqd_, tau_c_);

            for (size_t i = 0; i < n; ++i) {
                const double g = gravity_scale_ * tau_g_[i];
                const double tau =
                    inertia_scale_ * tau_m_[i] + coriolis_scale_ * tau_c_[i] + g;

                ff.name.push_back(names[i]);
                ff.position.push_back(qd_[i]);
                ff.velocity.push_back(dqd_[i]);
                ff.effort.push_back(tau);

                grav.name.push_back(names[i]);
                grav.position.push_back(q_[i]);
                grav.effort.push_back(g);
            }
        }

        if (!ff.name.empty()) {
            // Applied here so the ramp starts with the first torque actually sent.
            const double scale = feedforward_scale_ * Ramp(now);
            for (auto& e : ff.effort) e *= scale;
            for (auto& e : grav.effort) e *= scale;
            ff_pub_->publish(ff);
            gravity_pub_->publish(grav);
        }
    }

    // Start-up fade-in, 0 -> 1 over ramp_duration_ from the first call, then 1 for good.
    // Cosine shape: zero slope at both ends, so the torque neither jumps nor kinks.
    double Ramp(const rclcpp::Time& now) {
        if (ramp_done_) return 1.0;
        if (!ramp_start_) {
            ramp_start_ = now;
            RCLCPP_INFO(get_logger(), "Feedforward ramping in over %.2f s", ramp_duration_);
        }
        const double t = (now - *ramp_start_).seconds();
        if (ramp_duration_ <= 0.0 || t >= ramp_duration_) {
            ramp_done_ = true;
            RCLCPP_INFO(get_logger(), "Feedforward at full scale (feedforward_scale = %.3f)",
                        feedforward_scale_);
            return 1.0;
        }
        return 0.5 * (1.0 - std::cos(M_PI * std::max(t, 0.0) / ramp_duration_));
    }

    std::string root_link_;
    std::vector<std::string> tip_links_;
    std::vector<double> gravity_vector_;
    bool include_beyond_tip_;
    double gravity_scale_;
    double inertia_scale_;
    double coriolis_scale_;
    double desired_timeout_;
    double feedforward_scale_;
    double ramp_duration_;
    std::optional<rclcpp::Time> ramp_start_;
    bool ramp_done_ = false;

    std::vector<ChainModels> models_;
    std::unordered_map<std::string, Desired> desired_;
    std::unordered_map<std::string, double> positions_;
    std::vector<double> q_, qd_, dqd_, ddqd_, tau_g_, tau_m_, tau_c_;

    OnSetParametersCallbackHandle::SharedPtr param_cb_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr description_sub_;
    rclcpp::Subscription<JointTrajectory>::SharedPtr desired_sub_;
    rclcpp::Subscription<JointState>::SharedPtr joint_state_sub_;
    rclcpp::Publisher<JointState>::SharedPtr ff_pub_;
    rclcpp::Publisher<JointState>::SharedPtr gravity_pub_;
};

}  // namespace

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GravityCompensationNode>());
    rclcpp::shutdown();
    return 0;
}
