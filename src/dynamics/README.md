# dynamics

Robot dynamics computed with KDL from the URDF. The package has a single node,
`gravity_compensation_node`.

## gravity_compensation_node

```
/robot_description (std_msgs/String, latched)         URDF the KDL models are built from
/joint_states      (sensor_msgs/JointState)           measured q
/joint_desired     (trajectory_msgs/JointTrajectory)  desired q_d, dq_d, ddq_d (from joint_cmd)
-> /feedforward_torque (sensor_msgs/JointState)       effort = tau_ff
-> /gravity_torque     (sensor_msgs/JointState)       effort = G(q) term only (for debugging)
```

On every `/joint_states` message, for each chain `root_link -> tip_links[i]`:

```
tau_ff = feedforward_scale * ramp(t) * ( inertia_scale  * M(q_d) ddq_d       InertiaModel
                                       + coriolis_scale * C(q_d, dq_d) dq_d  CoriolisModel
                                       + gravity_scale  * G(q) )             GravityModel
```

**Start-up ramp:** at launch the arm is at rest and would otherwise be hit with the full torque at
once. `ramp(t)` goes smoothly from 0 to 1 over `ramp_duration` seconds (cosine shape, so there is
no jump or kink at either end), starting with the first torque the node publishes. It runs once:
after that it stays at 1, even if `/robot_description` is published again.

G uses the measured position, so it holds the arm where it really is. M and C use the desired
trajectory, so noisy measured velocity is never fed back through C. If a joint has had no
`/joint_desired` for longer than `desired_timeout`, it is taken as q_d = q and dq_d = ddq_d = 0,
which leaves only G. Torques are in URDF convention, with no hardware direction applied.
Links that hang below a tip (gripper, load) are lumped into the tip unless
`include_beyond_tip` is false.

| Parameter            | Default              | Notes                       |
|----------------------|----------------------|-----------------------------|
| `root_link`          | `base_link`          |                             |
| `tip_links`          | `[arm_link]`         | one chain per tip           |
| `gravity_vector`     | `[0.0, 0.0, -9.81]`  | gravity in `root_link`      |
| `include_beyond_tip` | `true`               |                             |
| `gravity_scale`      | `1.0`                | can be changed at runtime   |
| `inertia_scale`      | `1.0`                | can be changed at runtime   |
| `coriolis_scale`     | `1.0`                | can be changed at runtime   |
| `desired_timeout`    | `0.1` s              | can be changed at runtime   |
| `feedforward_scale`  | `1.0`                | overall factor, runtime     |
| `ramp_duration`      | `3.0` s              | start-up fade-in, 0 = off   |

```bash
ros2 run dynamics gravity_compensation_node --ros-args \
  --params-file $(ros2 pkg prefix dynamics)/share/dynamics/config/feedforward.yaml \
  --params-file <rig>.yaml
ros2 param set /gravity_compensation_node inertia_scale 0.0       # back to gravity only
ros2 param set /gravity_compensation_node feedforward_scale 0.5   # half of everything
```

## Layout

```
config/feedforward.yaml              ramp_duration, feedforward_scale, inertia/coriolis scales,
                                     desired_timeout (rig chain settings go in the rig's file)
include/dynamics/chain_model.hpp     BuildChain (URDF -> KDL chain) and the ChainModel base class
include/dynamics/gravity_model.hpp   GravityModel   G(q)          ChainDynParam::JntToGravity
include/dynamics/inertia_model.hpp   InertiaModel   M(q), M*ddq   ChainDynParam::JntToMass
include/dynamics/coriolis_model.hpp  CoriolisModel  C(q,dq)*dq    ChainDynParam::JntToCoriolis
src/gravity_compensation_node.cpp    the ROS 2 node (uses GravityModel)
test/test_gravity_model.cpp          G(q) against the analytic single-joint torque
test/test_dynamics_models.cpp        M and C against analytic values, KDL's RNE solver, and
                                     the dq' C dq = 1/2 dq' dM/dt dq identity
```

All three models take the same `(urdf_xml, root_link, tip_link, ..., include_beyond_tip)`
arguments, so together they give the full inverse dynamics:

```cpp
dynamics::InertiaModel  M(urdf, "base_link", "arm2_link");  M.Init();
dynamics::CoriolisModel C(urdf, "base_link", "arm2_link");  C.Init();
dynamics::GravityModel  G(urdf, "base_link", "arm2_link");  G.Init();

std::vector<double> tm, tc, tg;
M.ComputeInertialTorque(q_d, ddq_d, tm);   // M(q) * ddq_d
C.ComputeCoriolis(q_d, dq_d, tc);          // C(q, dq) * dq
G.ComputeGravity(q, tg);                   // G(q)
// tau_ff[i] = tm[i] + tc[i] + tg[i]
```

## Build and test

```bash
colcon build --packages-select dynamics
colcon test --packages-select dynamics && colcon test-result --verbose
```
