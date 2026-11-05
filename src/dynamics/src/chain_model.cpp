#include <urdf_parser/urdf_parser.h>

#include <dynamics/chain_model.hpp>
#include <fstream>
#include <kdl/tree.hpp>
#include <kdl_parser/kdl_parser.hpp>
#include <sstream>
#include <stdexcept>

namespace dynamics {

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

KDL::Chain BuildChain(const std::string& urdf_xml, const std::string& root_link,
                      const std::string& tip_link, bool include_beyond_tip) {
    auto urdf_model = urdf::parseURDF(urdf_xml);
    if (!urdf_model) {
        throw std::runtime_error("Failed to parse URDF");
    }
    KDL::Tree tree;
    if (!kdl_parser::treeFromUrdfModel(*urdf_model, tree)) {
        throw std::runtime_error("Failed to build KDL tree from URDF");
    }
    KDL::Chain chain;
    if (!tree.getChain(root_link, tip_link, chain)) {
        throw std::runtime_error("Failed to extract chain " + root_link + " -> " + tip_link);
    }
    if (!include_beyond_tip || chain.getNrOfSegments() == 0) {
        return chain;
    }

    const auto extra = SubtreeInertia(tree.getSegment(tip_link)->second);
    KDL::Chain merged;
    const auto last = chain.getNrOfSegments() - 1;
    for (unsigned int i = 0; i < last; ++i) merged.addSegment(chain.getSegment(i));
    const auto& tip = chain.getSegment(last);
    merged.addSegment(KDL::Segment(tip.getName(), tip.getJoint(), tip.getFrameToTip(),
                                   tip.getInertia() + extra));
    return merged;
}

std::vector<std::string> ChainJointNames(const KDL::Chain& chain) {
    std::vector<std::string> names;
    for (const auto& segment : chain.segments) {
        if (segment.getJoint().getType() != KDL::Joint::None) {
            names.push_back(segment.getJoint().getName());
        }
    }
    return names;
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

ChainModel::ChainModel(std::string urdf_xml, std::string root_link, std::string tip_link,
                       bool include_beyond_tip)
    : urdf_xml_(std::move(urdf_xml)),
      root_link_(std::move(root_link)),
      tip_link_(std::move(tip_link)),
      include_beyond_tip_(include_beyond_tip) {}

void ChainModel::InitChain() {
    chain_ = BuildChain(urdf_xml_, root_link_, tip_link_, include_beyond_tip_);
    joint_names_ = ChainJointNames(chain_);
}

void ChainModel::CheckSize(const std::vector<double>& v, const char* what) const {
    if (v.size() != NumJoints()) {
        throw std::invalid_argument(std::string(what) + " has " + std::to_string(v.size()) +
                                    " elements, chain has " + std::to_string(NumJoints()) +
                                    " joints");
    }
}

}  // namespace dynamics
