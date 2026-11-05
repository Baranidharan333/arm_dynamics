#include <dynamics/inertia_model.hpp>

namespace dynamics {

InertiaModel::InertiaModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                           bool include_beyond_tip)
    : ChainModel(std::move(urdf_xml), std::move(root_link), std::move(tip_link),
                 include_beyond_tip) {}

void InertiaModel::Init() {
    InitChain();

    const auto n = NumJoints();
    q_.resize(n);
    M_.resize(n);
    // Gravity does not enter M(q).
    solver_ = std::make_unique<KDL::ChainDynParam>(chain_, KDL::Vector::Zero());
}

void InertiaModel::ComputeMassMatrix(const std::vector<double>& q, Eigen::MatrixXd& M) {
    CheckSize(q, "q");
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) q_(i) = q[i];

    solver_->JntToMass(q_, M_);
    M = M_.data;
}

void InertiaModel::ComputeInertialTorque(const std::vector<double>& q,
                                         const std::vector<double>& ddq,
                                         std::vector<double>& tau) {
    CheckSize(q, "q");
    CheckSize(ddq, "ddq");
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) q_(i) = q[i];

    solver_->JntToMass(q_, M_);

    tau.resize(n);
    Eigen::Map<Eigen::VectorXd>(tau.data(), n) =
        M_.data * Eigen::Map<const Eigen::VectorXd>(ddq.data(), n);
}

}  // namespace dynamics
