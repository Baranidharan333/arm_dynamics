"""
utils.py
--------
Shared utility functions for the Joint Analyzer application.
Handles color coding thresholds, formatting helpers, and constants.

Note on joint names:
    Both /joint_cmd and the joint state topic are sensor_msgs/JointState.
    Joint names and count are discovered dynamically from the messages at
    runtime.  This file only holds the color/threshold logic and helpers.
"""

import math
from datetime import datetime
from typing import List

# ─── Difference Thresholds (radians) ─────────────────────────────────────────
THRESHOLD_GREEN  = math.radians(1.0)   # diff < 1.0° (approx 0.0175 rad)  → Green  🟢
THRESHOLD_YELLOW = math.radians(2.0)   # 1.0°–2.0° (approx 0.0349 rad)    → Yellow 🟡
# diff > 2.0° → Red 🔴

# ─── PyQt6 Color Strings ─────────────────────────────────────────────────────
COLOR_GREEN  = "#2ecc71"
COLOR_YELLOW = "#f1c40f"
COLOR_RED    = "#e74c3c"
COLOR_BG     = "#1e1e2e"   # dark background
COLOR_PANEL  = "#2a2a3e"   # panel/widget background
COLOR_TEXT   = "#cdd6f4"   # primary text
COLOR_MUTED  = "#6c7086"   # muted / secondary text
COLOR_ACCENT = "#89b4fa"   # accent blue


def diff_color(diff: float) -> str:
    """Return the hex color string for a given joint difference value."""
    if diff < THRESHOLD_GREEN:
        return COLOR_GREEN
    elif diff < THRESHOLD_YELLOW:
        return COLOR_YELLOW
    else:
        return COLOR_RED


def diff_emoji(diff: float) -> str:
    """Return a traffic-light emoji for a given joint difference value."""
    if diff < THRESHOLD_GREEN:
        return "🟢"
    elif diff < THRESHOLD_YELLOW:
        return "🟡"
    else:
        return "🔴"


def format_value(value: float, decimals: int = 4) -> str:
    """Format a float to a fixed number of decimal places."""
    return f"{value:.{decimals}f}"


def default_csv_filename() -> str:
    """Generate a timestamped default CSV filename."""
    ts = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"joint_analysis_{ts}.csv"


def format_duration(seconds: float) -> str:
    """Convert elapsed seconds into a human-readable MM:SS string."""
    mins = int(seconds) // 60
    secs = int(seconds) % 60
    return f"{mins:02d}:{secs:02d}"


class DiffStats:
    """Running avg/max of joint_cmd-vs-joint_states difference, accumulated
    in radians regardless of display unit."""

    def __init__(self):
        self.reset()

    def reset(self):
        self.names:    List[str]   = []
        self.diff_sum: List[float] = []
        self.max_diff: List[float] = []
        self.samples:  int         = 0

    def update(self, names: List[str], diffs: List[float]):
        if names != self.names:
            self.names    = names
            self.diff_sum = [0.0] * len(names)
            self.max_diff = [0.0] * len(names)
            self.samples  = 0
        self.samples += 1
        for i, d in enumerate(diffs):
            self.diff_sum[i] += d
            if d > self.max_diff[i]:
                self.max_diff[i] = d

    @property
    def avg(self) -> float:
        if self.samples == 0 or not self.diff_sum:
            return 0.0
        return sum(self.diff_sum) / (self.samples * len(self.diff_sum))

    @property
    def maximum(self) -> float:
        return max(self.max_diff) if self.max_diff else 0.0
