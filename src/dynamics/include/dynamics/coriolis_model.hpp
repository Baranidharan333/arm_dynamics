#pragma once

#include <dynamics/chain_model.hpp>
#include <kdl/chaindynparam.hpp>
#include <kdl/jntarray.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dynamics {

/*
 * Coriolis and centrifugal torque C(q, dq) * dq.
 * KDL ChainDynParam::JntToCoriolis (recursive Newton-Euler with ddq = 0 and no gravity).
 *
 * Quadratic in dq: centrifugal terms go with dq_i^2, Coriolis terms with dq_i * dq_j.
 * Zero at rest, and always zero for a single joint.
 */
class CoriolisModel : public ChainModel {
public:
    CoriolisModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                  bool include_beyond_tip = true);

    // Parses the URDF and builds the solver. Throws std::runtime_error on failure.
    void Init();

    // q and dq must have NumJoints() elements; tau is resized to match.
    void ComputeCoriolis(const std::vector<double>& q, const std::vector<double>& dq,
                         std::vector<double>& tau);

private:
    std::unique_ptr<KDL::ChainDynParam> solver_;
    KDL::JntArray q_;
    KDL::JntArray dq_;
    KDL::JntArray out_;
};

}  // namespace dynamics
