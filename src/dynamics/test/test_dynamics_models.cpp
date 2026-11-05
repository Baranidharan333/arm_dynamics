#include <gmock/gmock.h>

#include <Eigen/Eigenvalues>
#include <cmath>
#include <dynamics/coriolis_model.hpp>
#include <dynamics/gravity_model.hpp>
#include <dynamics/inertia_model.hpp>
#include <kdl/chainidsolver_recursive_newton_euler.hpp>
#include <stdexcept>
#include <vector>

using dynamics::CoriolisModel;
using dynamics::GravityModel;
using dynamics::InertiaModel;
using dynamics::ReadFile;

namespace {

constexpr double kTol = 1e-6;

// Inertia of arm + load about a joint axis (Y), from the URDF inertials:
//   bar:  iyy + m * L^2 = 5.8761e-4 + 0.150 * 0.1075^2
//   load: iyy + m * L^2 = 1.490625e-3 + 2.25 * 0.215^2
// Both rigs swing the same 215 mm arm + 2.25 kg load: joint1 of single_joint_1kg.urdf and
// joint2 of dual_joint_1kg.urdf.
constexpr double kArmLoadInertia = 5.8761e-4 + 0.150 * 0.1075 * 0.1075 + 1.490625e-3 +
                                   2.25 * 0.215 * 0.215;  // 0.1078179 kg m^2

}  // namespace

// ---------------------------------------------------------------------------------------------
// Single joint
// ---------------------------------------------------------------------------------------------

TEST(InertiaModel, SingleJointMatchesAnalyticInertia) {
    InertiaModel model(ReadFile(TEST_URDF), "base_link", "arm_link");
    model.Init();
    ASSERT_EQ(model.NumJoints(), 1u);

    Eigen::MatrixXd M;
    for (double q : {-1.0, 0.0, 0.5, 2.0}) {
        model.ComputeMassMatrix({q}, M);
        ASSERT_EQ(M.rows(), 1);
        ASSERT_EQ(M.cols(), 1);
        EXPECT_NEAR(M(0, 0), kArmLoadInertia, kTol) << "q = " << q;
    }

    std::vector<double> tau;
    model.ComputeInertialTorque({0.3}, {2.0}, tau);
    EXPECT_NEAR(tau[0], 2.0 * kArmLoadInertia, kTol);
}

TEST(CoriolisModel, SingleJointHasNoCoriolis) {
    CoriolisModel model(ReadFile(TEST_URDF), "base_link", "arm_link");
    model.Init();

    std::vector<double> tau;
    model.ComputeCoriolis({0.7}, {3.0}, tau);
    ASSERT_EQ(tau.size(), 1u);
    EXPECT_NEAR(tau[0], 0.0, kTol);
}

// ---------------------------------------------------------------------------------------------
// Two joints (coupled)
// ---------------------------------------------------------------------------------------------

class DualJoint : public testing::Test {
protected:
    void SetUp() override {
        urdf_ = ReadFile(TEST_DUAL_URDF);
        inertia_ = std::make_unique<InertiaModel>(urdf_, "base_link", "arm2_link");
        coriolis_ = std::make_unique<CoriolisModel>(urdf_, "base_link", "arm2_link");
        gravity_ = std::make_unique<GravityModel>(urdf_, "base_link", "arm2_link");
        inertia_->Init();
        coriolis_->Init();
        gravity_->Init();
    }

    std::string urdf_;
    std::unique_ptr<InertiaModel> inertia_;
    std::unique_ptr<CoriolisModel> coriolis_;
    std::unique_ptr<GravityModel> gravity_;
};

TEST_F(DualJoint, MassMatrixIsSymmetricPositiveDefinite) {
    ASSERT_EQ(inertia_->NumJoints(), 2u);
    EXPECT_THAT(inertia_->JointNames(), testing::ElementsAre("joint1", "joint2"));

    Eigen::MatrixXd M;
    for (double q2 : {-2.0, -0.5, 0.0, 1.0, 2.5}) {
        inertia_->ComputeMassMatrix({0.3, q2}, M);
        EXPECT_NEAR(M(0, 1), M(1, 0), kTol);
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(M);
        EXPECT_GT(eig.eigenvalues().minCoeff(), 0.0) << "q2 = " << q2;
        EXPECT_NEAR(M(1, 1), kArmLoadInertia, kTol) << "q2 = " << q2;
        // Coupling I2 + L1 * (m_bar * r_bar + m_load * r_load) * cos(q2), arm 1 is 0.270 m.
        EXPECT_NEAR(M(0, 1), kArmLoadInertia + 0.270 * 0.499875 * std::cos(q2), kTol)
            << "q2 = " << q2;
    }
}

TEST_F(DualJoint, MassMatrixDependsOnElbowOnly) {
    // Parallel axes: M depends on q2 (arm shape), not on q1 (where the whole arm points).
    Eigen::MatrixXd Ma, Mb, Mc;
    inertia_->ComputeMassMatrix({0.0, 0.8}, Ma);
    inertia_->ComputeMassMatrix({1.9, 0.8}, Mb);
    inertia_->ComputeMassMatrix({0.0, 0.0}, Mc);
    EXPECT_TRUE(Ma.isApprox(Mb, 1e-9));
    EXPECT_GT((Ma - Mc).norm(), 1e-3);
}

TEST_F(DualJoint, CoriolisIsQuadraticInVelocity) {
    const std::vector<double> q{0.4, 1.1};
    std::vector<double> tau0, tau1, tau2;
    coriolis_->ComputeCoriolis(q, {0.0, 0.0}, tau0);
    coriolis_->ComputeCoriolis(q, {1.5, -2.0}, tau1);
    coriolis_->ComputeCoriolis(q, {3.0, -4.0}, tau2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_NEAR(tau0[i], 0.0, kTol);
        EXPECT_NEAR(tau2[i], 4.0 * tau1[i], 1e-9);
    }
    EXPECT_GT(std::abs(tau1[0]) + std::abs(tau1[1]), 1e-3);
}

TEST_F(DualJoint, CoriolisPowerMatchesMassMatrixRate) {
    // dq' * C(q, dq) dq = 1/2 dq' dM/dt dq, with dM/dt = dM/dq * dq (finite difference).
    const std::vector<double> q{0.2, 0.9}, dq{1.3, -0.7};
    constexpr double h = 1e-6;
    Eigen::MatrixXd Mp, Mm;
    inertia_->ComputeMassMatrix({q[0] + h * dq[0], q[1] + h * dq[1]}, Mp);
    inertia_->ComputeMassMatrix({q[0] - h * dq[0], q[1] - h * dq[1]}, Mm);
    const Eigen::MatrixXd Mdot = (Mp - Mm) / (2 * h);
    const Eigen::Vector2d v(dq[0], dq[1]);

    std::vector<double> c;
    coriolis_->ComputeCoriolis(q, dq, c);
    EXPECT_NEAR(v.dot(Eigen::Vector2d(c[0], c[1])), 0.5 * v.dot(Mdot * v), 1e-6);
}

TEST_F(DualJoint, SumMatchesRecursiveNewtonEuler) {
    // M(q) ddq + C(q, dq) dq + G(q) must equal the full inverse dynamics from KDL's RNE solver.
    KDL::ChainIdSolver_RNE rne(inertia_->Chain(), KDL::Vector(0.0, 0.0, -9.81));

    const std::vector<std::vector<double>> cases[] = {
        {{0.0, 0.0}, {0.0, 0.0}, {1.0, -2.0}},
        {{0.5, -1.2}, {2.0, 1.0}, {0.0, 0.0}},
        {{-0.8, 2.1}, {-1.5, 3.0}, {4.0, -1.0}},
    };
    for (const auto& c : cases) {
        const auto& q = c[0];
        const auto& dq = c[1];
        const auto& ddq = c[2];

        std::vector<double> tm, tc, tg;
        inertia_->ComputeInertialTorque(q, ddq, tm);
        coriolis_->ComputeCoriolis(q, dq, tc);
        gravity_->ComputeGravity(q, tg);

        KDL::JntArray kq(2), kdq(2), kddq(2), ktau(2);
        for (unsigned i = 0; i < 2; ++i) {
            kq(i) = q[i];
            kdq(i) = dq[i];
            kddq(i) = ddq[i];
        }
        KDL::Wrenches f_ext(inertia_->Chain().getNrOfSegments(), KDL::Wrench::Zero());
        ASSERT_GE(rne.CartToJnt(kq, kdq, kddq, f_ext, ktau), 0);

        for (size_t i = 0; i < 2; ++i) {
            EXPECT_NEAR(tm[i] + tc[i] + tg[i], ktau(i), kTol)
                << "joint " << i << " q=(" << q[0] << ", " << q[1] << ")";
        }
    }
}

TEST_F(DualJoint, RejectsWrongSizes) {
    std::vector<double> tau;
    Eigen::MatrixXd M;
    EXPECT_THROW(inertia_->ComputeMassMatrix({0.0}, M), std::invalid_argument);
    EXPECT_THROW(inertia_->ComputeInertialTorque({0.0, 0.0}, {1.0}, tau), std::invalid_argument);
    EXPECT_THROW(coriolis_->ComputeCoriolis({0.0, 0.0}, {0.0, 0.0, 0.0}, tau),
                 std::invalid_argument);
    EXPECT_THROW(gravity_->ComputeGravity({0.0, 0.0, 0.0}, tau), std::invalid_argument);
}
