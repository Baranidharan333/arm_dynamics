#pragma once

#include <algorithm>
#include <cmath>

namespace posveleff_controllers
{

/*
 * Velocity/acceleration-limited tracking of a position target (trapezoidal profile).
 *
 * Each Step() moves the profile (pos, vel) toward target: it speeds up at max_acc, cruises at
 * max_vel, and brakes in time to stop at the target (|v| <= sqrt(2 * max_acc * distance)).
 * acc is the acceleration applied over the step, i.e. the desired acceleration for a model
 * feedforward.
 */
struct MotionLimiter
{
    // Final-approach snap window, in steps of max_acc * dt (see Step()).
    static constexpr double kSnapSteps = 6.0;

    double pos = 0.0;
    double vel = 0.0;
    double acc = 0.0;

    // Start at rest at position p.
    void Reset(double p)
    {
        pos = p;
        vel = 0.0;
        acc = 0.0;
    }

    void Step(double target, double dt, double max_vel, double max_acc)
    {
        if (!(dt > 0.0) || !(max_vel > 0.0) || !(max_acc > 0.0)) {
            acc = 0.0;
            return;
        }

        const double error = target - pos;
        const double max_dv = max_acc * dt;

        // Fastest speed that can still brake to zero at the target, for this discrete update
        // (pos += vel * dt, |dv| <= max_dv per step). From v = m * max_dv, braking covers
        // max_acc * dt^2 * m (m + 1) / 2, so m = (sqrt(1 + 8 d / (max_acc dt^2)) - 1) / 2.
        // The continuous v = sqrt(2 a d) brakes one step late and overshoots.
        const double m =
            0.5 * (std::sqrt(1.0 + 8.0 * std::abs(error) / (max_acc * dt * dt)) - 1.0);
        const double desired_vel = std::copysign(std::min(max_vel, m * max_dv), error);

        const double dv = std::clamp(desired_vel - vel, -max_dv, max_dv);
        vel += dv;
        acc = dv / dt;

        // Snap onto the target once this step would reach or pass it while approaching at
        // braking speed. The last bit of speed is dropped here; no acceleration is reported
        // for it. With a steady period the profile arrives at <= 2 * max_dv; a jittery
        // controller period (+-30%) raises that to ~5 * max_dv, hence the 6 * max_dv margin.
        // Anything faster cannot stop in time (e.g. the target jumped back) and overshoots.
        if (vel * error >= 0.0 && std::abs(error) <= std::abs(vel) * dt &&
            std::abs(vel) <= kSnapSteps * max_dv) {
            pos = target;
            vel = 0.0;
            acc = 0.0;
        } else {
            pos += vel * dt;
        }
    }
};

}  // namespace posveleff_controllers
