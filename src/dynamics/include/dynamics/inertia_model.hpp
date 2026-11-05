#pragma once

#include <Eigen/Core>
#include <dynamics/chain_model.hpp>
#include <kdl/chaindynparam.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/jntspaceinertiamatrix.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dynamics {

/*
 * Joint-space inertia (mass) matrix M(q), n x n, symmetric positive definite.
 * KDL ChainDynParam::JntToMass (composite rigid body algorithm).
 *
 * M(i, i) is the inertia joint i sees with the other joints held; M(i, j) couples them:
 * accelerating joint j needs torque M(i, j) * ddq(j) at joint i.
 */
class InertiaModel : public ChainModel {
public:
    InertiaModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                 bool include_beyond_tip = true);

    // Parses the URDF and builds the solver. Throws std::runtime_error on failure.
    void Init();

    // q must have NumJoints() elements; M is resized to NumJoints() x NumJoints().
    void ComputeMassMatrix(const std::vector<double>& q, Eigen::MatrixXd& M);

    // Inertial torque M(q) * ddq. q and ddq must have NumJoints() elements.
    void ComputeInertialTorque(const std::vector<double>& q, const std::vector<double>& ddq,
                               std::vector<double>& tau);

private:
    std::unique_ptr<KDL::ChainDynParam> solver_;
    KDL::JntArray q_;
    KDL::JntSpaceInertiaMatrix M_;
};

}  // namespace dynamics
