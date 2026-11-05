#include <gmock/gmock.h>

#include <cmath>
#include <gravity_compensation/gravity_model.hpp>
#include <stdexcept>

using gravity_compensation::GravityModel;
using gravity_compensation::ReadFile;

namespace {

// From the URDF header: tau_g(q) = g * (m_load * L_load + m_bar * L_bar) * sin(q)
// The URDF writes the +90 deg joint pitch as 1.5708, so the model is ~1e-5 Nm off exact.
constexpr double kTol = 1e-4;

double Expected(double q) { return 9.81 * (2.25 * 0.215 + 0.15 * 0.1075) * std::sin(q); }

}  // namespace

TEST(GravityModel, SingleJointMatchesAnalyticTorque) {
    GravityModel model(ReadFile(TEST_URDF), "base_link", "load_link");
    model.Init();

    ASSERT_EQ(model.NumJoints(), 1u);
    EXPECT_THAT(model.JointNames(), testing::ElementsAre("joint1"));

    std::vector<double> tau;
    for (double q : {-M_PI / 2, -M_PI / 4, 0.0, M_PI / 4, M_PI / 2, 2.0}) {
        model.ComputeGravity({q}, tau);
        ASSERT_EQ(tau.size(), 1u);
        EXPECT_NEAR(tau[0], Expected(q), kTol) << "q = " << q;
    }
}

TEST(GravityModel, LinksBeyondTipLoadTheChain) {
    // load_link hangs below arm_link, so ending the chain at arm_link must give the same torque.
    GravityModel model(ReadFile(TEST_URDF), "base_link", "arm_link");
    model.Init();

    std::vector<double> tau;
    model.ComputeGravity({0.7}, tau);
    EXPECT_NEAR(tau[0], Expected(0.7), kTol);
}

TEST(GravityModel, LinksBeyondTipCanBeIgnored) {
    GravityModel model(ReadFile(TEST_URDF), "base_link", "arm_link", {0.0, 0.0, -9.81}, false);
    model.Init();

    std::vector<double> tau;
    model.ComputeGravity({0.7}, tau);
    EXPECT_NEAR(tau[0], 9.81 * 0.15 * 0.1075 * std::sin(0.7), kTol);  // bar only
}

TEST(GravityModel, RejectsBadInput) {
    const auto urdf = ReadFile(TEST_URDF);
    EXPECT_THROW(GravityModel(urdf, "base_link", "no_such_link").Init(), std::runtime_error);
    EXPECT_THROW(GravityModel(urdf, "base_link", "load_link", {0.0, -9.81}).Init(),
                 std::runtime_error);
    EXPECT_THROW(GravityModel("<robot", "base_link", "load_link").Init(), std::runtime_error);
}
