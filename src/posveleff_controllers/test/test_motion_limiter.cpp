#include <gtest/gtest.h>

#include <cmath>

#include "posveleff_controllers/motion_limiter.hpp"

using posveleff_controllers::MotionLimiter;

namespace
{
constexpr double kDt = 0.01;  // 100 Hz, the controller_manager rate
constexpr double kVmax = 3.0;
constexpr double kAmax = 6.0;
}  // namespace

TEST(MotionLimiter, ReachesTargetWithinLimits)
{
    MotionLimiter lim;
    lim.Reset(0.0);
    double prev_vel = 0.0;
    double peak_vel = 0.0;
    int steps = 0;
    while ((lim.pos != 2.0 || lim.vel != 0.0) && steps < 1000)
    {
        lim.Step(2.0, kDt, kVmax, kAmax);
        EXPECT_LE(std::abs(lim.vel), kVmax + 1e-9);
        EXPECT_LE(std::abs(lim.acc), kAmax + 1e-9);
        // Reported acceleration is the velocity change over the step (except the final snap).
        if (lim.vel != 0.0) {
            EXPECT_NEAR(lim.acc, (lim.vel - prev_vel) / kDt, 1e-9);
        }
        EXPECT_LE(lim.pos, 2.0 + 1e-9) << "overshoot";
        peak_vel = std::max(peak_vel, lim.vel);
        prev_vel = lim.vel;
        ++steps;
    }
    EXPECT_DOUBLE_EQ(lim.pos, 2.0);
    EXPECT_DOUBLE_EQ(lim.vel, 0.0);
    EXPECT_NEAR(peak_vel, kVmax, 1e-9);
    // The speed dropped at the final snap is at most one step's dv (no hard stop).
    EXPECT_LE(prev_vel, kAmax * kDt + 1e-9);
    // Trapezoid: 2 x 0.5 s ramps + (2 - 1.5) / 3 s cruise = ~1.17 s.
    EXPECT_NEAR(steps * kDt, 1.17, 0.05);
}

TEST(MotionLimiter, MovesNegativeAndStaysAtTarget)
{
    MotionLimiter lim;
    lim.Reset(1.0);
    for (int i = 0; i < 300; ++i) lim.Step(-0.5, kDt, kVmax, kAmax);
    EXPECT_DOUBLE_EQ(lim.pos, -0.5);
    EXPECT_DOUBLE_EQ(lim.vel, 0.0);
    EXPECT_DOUBLE_EQ(lim.acc, 0.0);
}

TEST(MotionLimiter, TargetChangeMidMoveBrakesWithinLimit)
{
    MotionLimiter lim;
    lim.Reset(0.0);
    for (int i = 0; i < 40; ++i) lim.Step(3.0, kDt, kVmax, kAmax);
    ASSERT_GT(lim.vel, 1.0);
    // Reverse: velocity may only change by max_acc * dt per step.
    double prev = lim.vel;
    for (int i = 0; i < 300; ++i)
    {
        lim.Step(0.0, kDt, kVmax, kAmax);
        if (lim.vel != 0.0) {
            EXPECT_LE(std::abs(lim.vel - prev), kAmax * kDt + 1e-9);
        }
        prev = lim.vel;
    }
    EXPECT_DOUBLE_EQ(lim.pos, 0.0);
}

TEST(MotionLimiter, NoOvershootWithJitteryPeriod)
{
    // controller_manager periods are not exactly 10 ms; deterministic +-30% jitter.
    for (double target : {0.05, 0.4, 1.5, -2.0})
    {
        MotionLimiter lim;
        lim.Reset(0.0);
        for (int i = 0; i < 1000; ++i)
        {
            const double dt = kDt * (1.0 + 0.3 * std::sin(i * 1.7));
            lim.Step(target, dt, kVmax, kAmax);
            EXPECT_LE(std::abs(lim.pos), std::abs(target) + 1e-9) << "target " << target;
            EXPECT_LE(std::abs(lim.acc), kAmax + 1e-9);
        }
        EXPECT_DOUBLE_EQ(lim.pos, target);
        EXPECT_DOUBLE_EQ(lim.vel, 0.0);
    }
}

TEST(MotionLimiter, InvalidInputsHoldState)
{
    MotionLimiter lim;
    lim.Reset(0.3);
    lim.Step(1.0, 0.0, kVmax, kAmax);
    lim.Step(1.0, kDt, 0.0, kAmax);
    lim.Step(1.0, kDt, kVmax, -1.0);
    EXPECT_DOUBLE_EQ(lim.pos, 0.3);
    EXPECT_DOUBLE_EQ(lim.vel, 0.0);
}
