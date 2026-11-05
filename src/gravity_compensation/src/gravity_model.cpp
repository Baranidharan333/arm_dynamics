#include <urdf_parser/urdf_parser.h>

#include <fstream>
#include <gravity_compensation/gravity_model.hpp>
#include <kdl/tree.hpp>
#include <kdl_parser/kdl_parser.hpp>
#include <sstream>
#include <stdexcept>

namespace gravity_compensation {

namespace {

// Inertia of everything below element, in element's frame, with movable joints at 0.
KDL::RigidBodyInertia SubtreeInertia(const KDL::TreeElement& element) {
    KDL::RigidBodyInertia total = KDL::RigidBodyInertia::Zero();
    for (const auto& child_it : GetTreeElementChildren(element)) {
        const auto& child = child_it->second;
        const auto& segment = GetTreeElementSegment(child);
        total = total + segment.pose(0.0) * (segment.getInertia() + SubtreeInertia(child));
    }
    return total;
}

}  // namespace

GravityModel::GravityModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                           std::vector<double> gravity_vector, bool include_beyond_tip)
    : urdf_xml_(std::move(urdf_xml)),
      root_link_(std::move(root_link)),
      tip_link_(std::move(tip_link)),
      gravity_vector_(std::move(gravity_vector)),
      include_beyond_tip_(include_beyond_tip) {}

void GravityModel::Init() {
    if (gravity_vector_.size() != 3) {
        throw std::runtime_error("gravity_vector must have 3 elements");
    }

    auto urdf_model = urdf::parseURDF(urdf_xml_);
    if (!urdf_model) {
        throw std::runtime_error("Failed to parse URDF");
    }
    KDL::Tree tree;
    if (!kdl_parser::treeFromUrdfModel(*urdf_model, tree)) {
        throw std::runtime_error("Failed to build KDL tree from URDF");
    }
    chain_ = KDL::Chain();
    if (!tree.getChain(root_link_, tip_link_, chain_)) {
        throw std::runtime_error("Failed to extract chain " + root_link_ + " -> " + tip_link_);
    }

    if (include_beyond_tip_ && chain_.getNrOfSegments() > 0) {
        const auto extra = SubtreeInertia(tree.getSegment(tip_link_)->second);
        KDL::Chain merged;
        const auto last = chain_.getNrOfSegments() - 1;
        for (unsigned int i = 0; i < last; ++i) merged.addSegment(chain_.getSegment(i));
        const auto& tip = chain_.getSegment(last);
        merged.addSegment(KDL::Segment(tip.getName(), tip.getJoint(), tip.getFrameToTip(),
                                       tip.getInertia() + extra));
        chain_ = merged;
    }

    const auto n = chain_.getNrOfJoints();
    q_.resize(n);
    dq_.resize(n);
    out_.resize(n);

    solver_ = std::make_unique<KDL::ChainDynParam>(
        chain_, KDL::Vector(gravity_vector_[0], gravity_vector_[1], gravity_vector_[2]));
}

std::vector<std::string> GravityModel::JointNames() const {
    std::vector<std::string> names;
    for (const auto& segment : chain_.segments) {
        if (segment.getJoint().getType() != KDL::Joint::None) {
            names.push_back(segment.getJoint().getName());
        }
    }
    return names;
}

void GravityModel::ComputeGravity(const std::vector<double>& q, std::vector<double>& tau) {
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) q_(i) = q[i];

    solver_->JntToGravity(q_, out_);

    tau.resize(n);
    for (size_t i = 0; i < n; ++i) tau[i] = out_(i);
}

void GravityModel::ComputeCoriolis(const std::vector<double>& q, const std::vector<double>& dq,
                                   std::vector<double>& tau) {
    const auto n = NumJoints();
    for (size_t i = 0; i < n; ++i) {
        q_(i) = q[i];
        dq_(i) = dq[i];
    }

    solver_->JntToCoriolis(q_, dq_, out_);

    tau.resize(n);
    for (size_t i = 0; i < n; ++i) tau[i] = out_(i);
}

std::string ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}  // namespace gravity_compensation
