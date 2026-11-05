"""
recorder.py
-----------
Recording: captures joint state samples (best-effort) from follower command and follower state,
and exports a clean, well-labelled CSV report.
"""

import csv
import math
import time
from dataclasses import dataclass
from typing import Dict, List, Optional


@dataclass
class JointSample:
    """Positions and differences for 2-topic joint comparison at one point in time."""
    timestamp: float
    joint_names: List[str]
    cmd:         List[Optional[float]]
    actual:      List[Optional[float]]


class Recorder:
    """
    Joint State Recorder supporting 2-topic target-vs-actual logging.
    """

    def __init__(self) -> None:
        self._recording:   bool             = False
        self._samples:     List[JointSample] = []
        self._start_time:  Optional[float]  = None

    def start(self) -> None:
        """Begin a new recording session (clears previous data)."""
        self._samples.clear()
        self._recording  = True
        self._start_time = time.time()

    def stop(self) -> None:
        """Pause recording (data is preserved in memory)."""
        self._recording = False

    def clear(self) -> None:
        """Discard all data and reset the recorder."""
        self._samples.clear()
        self._recording   = False
        self._start_time  = None

    def update(
        self,
        joint_names: List[str],
        cmd:         List[Optional[float]],
        actual:      List[Optional[float]],
    ) -> None:
        """
        Record a 2-topic sample when active.
        """
        if self._recording:
            self._samples.append(JointSample(
                timestamp   = time.time() - self._start_time,
                joint_names = list(joint_names),
                cmd         = list(cmd),
                actual      = list(actual),
            ))

    def save_csv(self, filepath: str, use_degrees: bool = False) -> int:
        """
        Write all recorded samples to a CSV file.

        Returns
        -------
        int : number of data rows written
        """
        if not self._samples:
            return 0

        # Discover all joint names across all samples to construct columns consistently
        all_joints = []
        seen = set()
        for sample in self._samples:
            for name in sample.joint_names:
                if name not in seen:
                    seen.add(name)
                    all_joints.append(name)
        all_joints.sort()

        # Build header
        unit = "deg" if use_degrees else "rad"
        header = ["Relative Time (s)"]
        for name in all_joints:
            header += [
                f"{name}_target ({unit})",
                f"{name}_actual ({unit})",
                f"{name}_difference ({unit})"
            ]

        def fmt(val: Optional[float]) -> str:
            if val is None:
                return ""
            if use_degrees:
                val = val * 180.0 / math.pi
            return f"{val:.6f}"

        with open(filepath, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(header)

            for sample in self._samples:
                row = [f"{sample.timestamp:.6f}"]
                name_to_idx = {n: i for i, n in enumerate(sample.joint_names)}
                for name in all_joints:
                    if name in name_to_idx:
                        i = name_to_idx[name]
                        c_val = sample.cmd[i]
                        a_val = sample.actual[i]
                        diff = abs(c_val - a_val) if c_val is not None and a_val is not None else None

                        row += [
                            fmt(c_val),
                            fmt(a_val),
                            fmt(diff)
                        ]
                    else:
                        row += ["", "", ""]
                writer.writerow(row)

        return len(self._samples)

    @property
    def is_recording(self) -> bool:
        return self._recording

    @property
    def sample_count(self) -> int:
        return len(self._samples)

    @property
    def elapsed_seconds(self) -> float:
        if self._start_time is None:
            return 0.0
        return time.time() - self._start_time
