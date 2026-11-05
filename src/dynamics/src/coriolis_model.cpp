#include <dynamics/coriolis_model.hpp>

namespace dynamics {

CoriolisModel::CoriolisModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                             bool include_beyond_tip)
    : ChainModel(std::move(urdf_xml), std::move(root_link), std::move(tip_link),
                 include_beyond_tip) {}

void CoriolisModel::Init() {
    InitChain();

    const auto n = NumJoints();
    q_.resize(n);
    dq_.resize(n);
    out_.resize(n);
    // Gravity does not enter C(q, dq) * dq.
    solver_ = std::make_unique<KDL::ChainDynParam>(chain_, KDL::Vector::Zero());
}

void CoriolisModel::ComputeCoriolis(const std::vector<double>& q, const std::vector<double>& dq,
                                    std::vector<double>& tau) {
    CheckSize(q, "q");
    CheckSize(dq, "dq");
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) {
        q_(i) = q[i];
        dq_(i) = dq[i];
    }

    solver_->JntToCoriolis(q_, dq_, out_);

    tau.resize(n);
    for (size_t i = 0; i < n; ++i) tau[i] = out_(i);
}

}  // namespace dynamics
