#!/usr/bin/env python3
"""Empirical Roofline Model Plotter.

Constructs an empirical Roofline analysis figure for the 2D and 3D heat diffusion
stencil computations. Maps operational arithmetic intensity (FLOPs/byte) against
sustained GFLOP/s, comparing measured throughput with host hardware memory bandwidth
and peak vector compute bounds.
"""

import sys
from pathlib import Path

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import numpy as np
    import pandas as pd
    HAS_DEPS = True
except ImportError:
    HAS_DEPS = False


def generate_roofline():
    if not HAS_DEPS:
        print("[ADVISORY] matplotlib and pandas are required to render roofline plots.")
        print("           Install dependencies via: pip install matplotlib pandas")
        return
    results_dir = Path(__file__).resolve().parent.parent / "results" / "g5"
    figures_dir = Path(__file__).resolve().parent / "figures"
    figures_dir.mkdir(parents=True, exist_ok=True)

    file_a = results_dir / "g5_summary_A.csv"
    file_b = results_dir / "g5_summary_B.csv"

    # Hardware constants for Intel Core i5-12500H P-cores (Alder Lake)
    # 4 P-cores @ ~3.5 GHz AVX2 base/all-core: ~448 GFLOP/s double precision peak
    PEAK_GFLOPS = 448.0
    # Dual-channel DDR4/DDR5 peak practical stream bandwidth: ~55 GB/s
    MEM_BANDWIDTH_GBS = 55.0

    # Operational intensities (FLOPs / byte)
    # 2D 5-point stencil: 6 FLOPs / (6 * 8 bytes) = 0.125 FLOPs/byte compulsory
    AI_2D = 0.125
    # 3D 7-point stencil: 8 FLOPs / (8 * 8 bytes) = 0.125 FLOPs/byte compulsory
    AI_3D = 0.125

    fig, ax = plt.subplots(figsize=(9, 6), dpi=150)

    # Arithmetic intensity axis range
    x_ai = np.logspace(-2, 2, 400)
    # Roofline ceiling: min(Peak_GFLOPs, Memory_Bandwidth * AI)
    y_ceiling = np.minimum(PEAK_GFLOPS, MEM_BANDWIDTH_GBS * x_ai)

    ax.loglog(x_ai, y_ceiling, "k-", linewidth=2.2, label=f"Roofline Ceiling ({MEM_BANDWIDTH_GBS} GB/s DRAM, {PEAK_GFLOPS} GFLOP/s)")
    ax.axhline(PEAK_GFLOPS, color="gray", linestyle="--", alpha=0.5, label=f"Peak Compute ({PEAK_GFLOPS} GFLOP/s)")

    # Plot memory bandwidth sloped asymptote
    mem_line = MEM_BANDWIDTH_GBS * x_ai
    ax.plot(x_ai[mem_line <= PEAK_GFLOPS], mem_line[mem_line <= PEAK_GFLOPS], "b--", alpha=0.4, label="Memory Bandwidth Bound")

    # Plot Gate 5 Empirical Data Points if present
    markers = {"mpi": "o", "omp": "s", "hybrid": "^"}
    colors_2d = ["#2b5c8f", "#3c8dbc", "#00a65a"]
    colors_3d = ["#d81b60", "#f39c12", "#605ca8"]

    if file_a.exists():
        df_a = pd.read_csv(file_a)
        for _, row in df_a.iterrows():
            # 2D: 6 FLOPs per grid point update
            gflops = (row["mups"] * 6.0) / 1000.0
            lbl_type = "mpi" if "mpi" in row["label"] else ("omp" if "omp" in row["label"] else "hybrid")
            ax.scatter(AI_2D, gflops, color="#1f77b4", s=90, zorder=5,
                       marker=markers[lbl_type], edgecolors="black", linewidth=0.8)
            ax.annotate(f"2D {row['label']} ({row['cores']:.0f}c)",
                        (AI_2D, gflops), textcoords="offset points", xytext=(8, -3), fontsize=7.5)

    if file_b.exists():
        df_b = pd.read_csv(file_b)
        for _, row in df_b.iterrows():
            # 3D: 8 FLOPs per grid point update
            gflops = (row["mups"] * 8.0) / 1000.0
            lbl_type = "mpi" if "mpi" in row["label"] else ("omp" if "omp" in row["label"] else "hybrid")
            ax.scatter(AI_3D, gflops, color="#e65100", s=90, zorder=5,
                       marker=markers[lbl_type], edgecolors="black", linewidth=0.8)
            ax.annotate(f"3D {row['label']} ({row['cores']:.0f}c)",
                        (AI_3D, gflops), textcoords="offset points", xytext=(8, -3), fontsize=7.5)

    # Annotate Bandwidth Bound Region
    ax.text(0.015, 0.4, "Strictly Memory Bandwidth Bound\n(All configurations below 0.5 FLOPs/byte)",
            fontsize=9.5, bbox=dict(boxstyle="round,pad=0.5", facecolor="#fff9c4", alpha=0.8, edgecolor="#fbc02d"))

    ax.set_xlim(0.01, 10.0)
    ax.set_ylim(0.1, 800.0)
    ax.set_xlabel("Arithmetic Intensity [FLOPs / Byte]", fontsize=11, fontweight="bold")
    ax.set_ylabel("Attainable Performance [GFLOP/s]", fontsize=11, fontweight="bold")
    ax.set_title("Empirical Roofline Analysis: 2D & 3D Stencil Scaling (Intel Alder Lake P-Cores)",
                 fontsize=12, fontweight="bold", pad=12)
    ax.grid(True, which="both", linestyle=":", alpha=0.6)
    ax.legend(loc="upper left", fontsize=8.5, framealpha=0.9)

    out_path = figures_dir / "g5_empirical_roofline.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=160)
    plt.close()
    print(f"Generated empirical roofline figure: {out_path}")


if __name__ == "__main__":
    generate_roofline()
