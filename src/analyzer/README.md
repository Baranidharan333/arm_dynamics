# Joint Command vs Joint State Analyzer

A lightweight ROS 2 web tool for comparing commanded joint positions with actual positions.

---

## What It Does

- Subscribes to **2 `sensor_msgs/JointState` topics**:
  - `/joint_cmd` — commanded joint positions
  - `/joint_states` — actual joint positions
- Matches joints **by name** automatically
- Live **joint comparison table** with color-coded differences, served as a web page
- **Statistics** (average diff, max diff, samples)
- **Records** samples and exports a clean CSV file

---

## Requirements

| Requirement | Version |
|-------------|---------|
| ROS 2 | Humble / Iron / Jazzy |
| Python | 3.8 + |
| Flask | 3.0 + |

---

## Quick Start

This is a colcon package (`joint_analyzer`, ament_python). Build it once from
the workspace root:

```bash
source /opt/ros/jazzy/setup.bash
colcon build --packages-select joint_analyzer
source install/setup.bash
```

Then either run the comparison table on its own:

```bash
ros2 run joint_analyzer server --host 0.0.0.0 --port 8130
```

or launch it together with the generic live plotter (`plot_server`, port 8131):

```bash
ros2 launch joint_analyzer joint_analyzer.launch.py
```

Open **http://<host>:8130/** (and **:8131/** for the plotter) in a browser.
`host`/`server_port`/`plot_port` are launch arguments if you need different
values, e.g. `ros2 launch joint_analyzer joint_analyzer.launch.py server_port:=9000`.

The standalone diagnostic scripts are also installed as executables:
`ros2 run joint_analyzer check_diffs`, `diagnose_qos`, `verify_mapping`.

---

## How to Use

### Connect

The two topic fields (with autocomplete from currently advertised
`JointState` topics) pre-fill your robot's topics:

| Field | Pre-filled value |
|-------|-------------------|
| Follower Command | `/joint_cmd` |
| Follower Actual | `/joint_states` |

Click **Connect**. The status line turns green once data flows.

### Live Table

Shows, per joint:

| Column | Description |
|--------|-------------|
| **Joint Name** | Joint name exactly as published on the topic |
| **joint_cmd** | Commanded position |
| **joint_states** | Actual sensor position |
| **Difference** | `abs(cmd − actual)`, color-coded |

### Color Guide

| Color | Threshold (Degrees) | Threshold (Radians) |
|-------|---------------------|----------------------|
| 🟢 Green | diff < 1.0° | diff < 0.0175 rad |
| 🟡 Yellow | 1.0° – 2.0° | 0.0175 – 0.0349 rad |
| 🔴 Red | diff > 2.0° | diff > 0.0349 rad |

### Recording & CSV

1. Click **⏺ Start Recording**
2. Move the arm / run teleoperation
3. Click **⏹ Stop Recording**
4. Click **💾 Save CSV** — downloads through the browser

### CSV Format

```
Relative Time (s) | joint1_target | joint1_actual | joint1_diff | ...
```

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| No topics in the autocomplete list | Source ROS 2 and ensure topics are publishing |
| `● Waiting for data…` | Run `ros2 topic hz /joint_cmd` to verify publishing |
| Table stays empty | Joint names must match between cmd and state topics |
| `rclpy` not found | `source /opt/ros/<distro>/setup.bash` before running `server.py` |
| Page loads but never connects | Check the ROS Domain ID field matches the robot's |
