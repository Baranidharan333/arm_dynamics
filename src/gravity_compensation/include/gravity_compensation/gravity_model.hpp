#pragma once

#include <kdl/chain.hpp>
#include <kdl/chaindynparam.hpp>
#include <kdl/jntarray.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gravity_compensation {

/*
 * Computes joint-space gravity torques tau_g(q) for a serial chain
 * extracted from a URDF, using KDL's recursive Newton-Euler solver.
 *
 * Links hanging below the tip (hand, gripper, load) are lumped into the tip, with any movable
 * joints out there taken at 0, unless include_beyond_tip is false.
 */
class GravityModel {
public:
    GravityModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                 std::vector<double> gravity_vector = {0.0, 0.0, -9.81},
                 bool include_beyond_tip = true);

    // Parses the URDF and builds the solver. Throws std::runtime_error on failure.
    void Init();

    size_t NumJoints() const { return chain_.getNrOfJoints(); }
    std::vector<std::string> JointNames() const;
    const std::string& RootLink() const { return root_link_; }
    const std::string& TipLink() const { return tip_link_; }

    // q and tau must have NumJoints() elements.
    void ComputeGravity(const std::vector<double>& q, std::vector<double>& tau);
    void ComputeCoriolis(const std::vector<double>& q, const std::vector<double>& dq,
                         std::vector<double>& tau);

private:
    std::string urdf_xml_;
    std::string root_link_;
    std::string tip_link_;
    std::vector<double> gravity_vector_;
    bool include_beyond_tip_;

    KDL::Chain chain_;
    std::unique_ptr<KDL::ChainDynParam> solver_;

    KDL::JntArray q_;
    KDL::JntArray dq_;
    KDL::JntArray out_;
};

// Reads a whole file into a string. Throws std::runtime_error if it cannot be opened.
std::string ReadFile(const std::string& path);

}  // namespace gravity_compensation
