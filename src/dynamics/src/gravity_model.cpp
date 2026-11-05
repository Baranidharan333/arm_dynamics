#include <dynamics/gravity_model.hpp>
#include <stdexcept>

namespace dynamics {

GravityModel::GravityModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                           std::vector<double> gravity_vector, bool include_beyond_tip)
    : ChainModel(std::move(urdf_xml), std::move(root_link), std::move(tip_link),
                 include_beyond_tip),
      gravity_vector_(std::move(gravity_vector)) {}

void GravityModel::Init() {
    if (gravity_vector_.size() != 3) {
        throw std::runtime_error("gravity_vector must have 3 elements");
    }
    InitChain();

    const auto n = NumJoints();
    q_.resize(n);
    out_.resize(n);
    solver_ = std::make_unique<KDL::ChainDynParam>(
        chain_, KDL::Vector(gravity_vector_[0], gravity_vector_[1], gravity_vector_[2]));
}

void GravityModel::ComputeGravity(const std::vector<double>& q, std::vector<double>& tau) {
    CheckSize(q, "q");
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) q_(i) = q[i];

    solver_->JntToGravity(q_, out_);

    tau.resize(n);
    for (size_t i = 0; i < n; ++i) tau[i] = out_(i);
}

}  // namespace dynamics
