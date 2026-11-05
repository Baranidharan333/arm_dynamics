# gravity_compensation

Standalone gravity compensation for one OpenArm (7-DOF arm + gripper). With this running, the arm
holds its own weight and can be moved freely by hand.

## How it works

Each control cycle:

1. Read joint positions `q` and velocities `dq` from the motors over CAN.
2. Compute the gravity torque `tau_g(q)` from the URDF using KDL (`ChainDynParam::JntToGravity`).
3. Optionally add friction feedforward (tanh model) and Coriolis feedforward.
4. Scale by a start-up ramp, clamp to per-joint torque limits, and send in MIT mode:
   `kp = 0`, `kd = damping`, `tau_ff = tau`.

Compared to `openarm_teleop/control/gravity_compasation.cpp`, this adds a fixed-rate loop,
torque clamping, a soft-start ramp, optional damping/friction/Coriolis terms, a configurable
gravity vector, gripper damping, and motor disable on SIGINT/SIGTERM or errors.

## Layout

```
config/gravity_comp.yaml        tuning parameters
include/gravity_compensation/   GravityModel (KDL) and Config
src/gravity_model.cpp           URDF -> KDL chain -> gravity/Coriolis torques
src/config.cpp                  YAML loader
src/gravity_comp_node.cpp       real-time loop driving the motors
src/gravity_check.cpp           offline tool: prints tau_g for a pose, no hardware
scripts/launch.sh               xacro -> URDF, then runs the node
```

## Dependencies

Same as `openarm_teleop`: OpenArmCAN, orocos_kdl, kdl_parser, urdfdom, Eigen3, yaml-cpp.
`xacro` and `openarm_description` (in a ROS 2 workspace) are needed to generate the URDF.

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

## Run

```bash
# Bring up CAN first, e.g.
sudo ip link set can0 up type can bitrate 1000000

# Generates the URDF and starts compensation (defaults: right_arm can0 v10)
scripts/launch.sh right_arm can0 v10

# Or run directly with your own URDF
./build/gravity_comp_node right_arm can0 /path/to/v10_bimanual.urdf config/gravity_comp.yaml
```

Check the model offline before powering the arm:

```bash
./build/gravity_check /path/to/v10_bimanual.urdf right_arm 0 0.5 0 1.2 0 0 0
```

## Tuning

- **Arm sags:** the model is too light. Raise `gravity_scale` slightly or fix the URDF masses.
- **Arm drifts up:** the model is too heavy. Lower `gravity_scale`.
- **Arm creeps or feels "sticky":** adjust `friction_scale`. The `Fo` bias acts even at rest.
- **Oscillation or slow drift:** increase `arm.damping`.
- **Base not upright:** set `model.gravity_vector` to gravity expressed in `root_link`.

Ctrl+C disables all motors. The arm will then fall under gravity, so support it before stopping.
