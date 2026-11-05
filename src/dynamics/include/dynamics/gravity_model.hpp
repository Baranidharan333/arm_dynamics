#pragma once

#include <dynamics/chain_model.hpp>
#include <kdl/chaindynparam.hpp>
#include <kdl/jntarray.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dynamics {

/*
 * Gravity torque G(q): the joint torques that hold the chain still against gravity.
 * KDL ChainDynParam::JntToGravity (recursive Newton-Euler).
 */
class GravityModel : public ChainModel {
public:
    GravityModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                 std::vector<double> gravity_vector = {0.0, 0.0, -9.81},
                 bool include_beyond_tip = true);

    // Parses the URDF and builds the solver. Throws std::runtime_error on failure.
    void Init();

    // q must have NumJoints() elements; tau is resized to match.
    void ComputeGravity(const std::vector<double>& q, std::vector<double>& tau);

private:
    std::vector<double> gravity_vector_;
    std::unique_ptr<KDL::ChainDynParam> solver_;
    KDL::JntArray q_;
    KDL::JntArray out_;
};

}  // namespace dynamics
