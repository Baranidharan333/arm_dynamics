#pragma once

#include <kdl/chain.hpp>
#include <string>
#include <vector>

namespace dynamics {

/*
 * Builds the serial chain root_link -> tip_link from a URDF.
 *
 * Links hanging below the tip (hand, gripper, load) are lumped into the tip, with any movable
 * joints out there taken at 0, unless include_beyond_tip is false.
 * Throws std::runtime_error on failure.
 */
KDL::Chain BuildChain(const std::string& urdf_xml, const std::string& root_link,
                      const std::string& tip_link, bool include_beyond_tip = true);

// Names of the movable joints of chain, root to tip.
std::vector<std::string> ChainJointNames(const KDL::Chain& chain);

// Reads a whole file into a string. Throws std::runtime_error if it cannot be opened.
std::string ReadFile(const std::string& path);

/*
 * Common part of the dynamics models: holds the URDF chain and its joint names.
 * Derived classes build their KDL solver in Init().
 */
class ChainModel {
public:
    size_t NumJoints() const { return chain_.getNrOfJoints(); }
    const std::vector<std::string>& JointNames() const { return joint_names_; }
    const std::string& RootLink() const { return root_link_; }
    const std::string& TipLink() const { return tip_link_; }
    const KDL::Chain& Chain() const { return chain_; }

protected:
    ChainModel(std::string urdf_xml, std::string root_link, std::string tip_link,
               bool include_beyond_tip);

    // Builds chain_ and joint_names_. Throws std::runtime_error on failure.
    void InitChain();

    // Throws std::invalid_argument unless v has NumJoints() elements.
    void CheckSize(const std::vector<double>& v, const char* what) const;

    KDL::Chain chain_;

private:
    std::string urdf_xml_;
    std::string root_link_;
    std::string tip_link_;
    bool include_beyond_tip_;
    std::vector<std::string> joint_names_;
};

}  // namespace dynamics
