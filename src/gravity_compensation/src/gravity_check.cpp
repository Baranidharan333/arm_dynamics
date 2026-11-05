// Offline check: prints the gravity torque the model predicts for a joint pose.
// No ROS graph / hardware needed. Use it to sanity-check the URDF before powering the arm.

#include <gravity_compensation/gravity_model.hpp>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <urdf_path> <left_arm|right_arm|ROOT:TIP> [q1 ... qN]\n"
                  << "Joint angles in radians; omitted joints default to 0.\n"
                  << "Example: " << argv[0] << " single_joint_1kg.urdf base_link:arm_link 0"
                  << std::endl;
        return 1;
    }

    const std::string urdf_path = argv[1];
    const std::string chain_arg = argv[2];

    // left_arm / right_arm: the daksha arms, gripper included.
    std::string root = "base_link";
    std::string tip;
    if (chain_arg == "left_arm") {
        tip = "left_link_8";
    } else if (chain_arg == "right_arm") {
        tip = "right_link_8";
    } else if (const auto sep = chain_arg.find(':'); sep != std::string::npos) {
        root = chain_arg.substr(0, sep);
        tip = chain_arg.substr(sep + 1);
    } else {
        std::cerr << "[ERROR] Expected left_arm, right_arm or ROOT:TIP, got: " << chain_arg
                  << std::endl;
        return 1;
    }

    try {
        gravity_compensation::GravityModel model(gravity_compensation::ReadFile(urdf_path), root,
                                                 tip);
        model.Init();

        std::vector<double> q(model.NumJoints(), 0.0);
        for (int i = 3; i < argc && static_cast<size_t>(i - 3) < q.size(); ++i) {
            q[i - 3] = std::stod(argv[i]);
        }

        std::vector<double> tau;
        model.ComputeGravity(q, tau);

        const auto names = model.JointNames();
        std::cout << std::fixed << std::setprecision(4);
        for (size_t i = 0; i < tau.size(); ++i) {
            std::cout << std::setw(28) << std::left << (i < names.size() ? names[i] : "?")
                      << " q=" << std::setw(9) << std::right << q[i] << "  tau_g=" << std::setw(9)
                      << tau[i] << " Nm\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
