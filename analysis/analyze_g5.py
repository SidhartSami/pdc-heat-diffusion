#!/usr/bin/env python3
"""Gate 5 analysis: strong/weak scaling tables, overlap bootstrap test, and figures.

Implements the statistics pre-registered in docs/g5_scaling_plan.md:
  * primary metric = median wall time over the recorded runs
  * dispersion     = full range and IQR
  * S(p) = T(1)/T(p),  E(p) = S(p)/p
  * overlap        = median paired difference and a 95% bootstrap CI of the paired
                     differences; "improvement" iff median(delta) < 0 AND CI excludes 0.

Usage: python analysis/analyze_g5.py [results_dir] [figures_dir]
"""
import json
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
import pandas as pd  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
RES = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "results" / "g5"
FIG = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "analysis" / "figures"
FIG.mkdir(parents=True, exist_ok=True)

BOOT = 20000
RNG = np.random.default_rng(12345)  # fixed seed -> reproducible CI


def load(suite: str) -> pd.DataFrame:
    path = RES / f"g5_suite_{suite}.csv"
    if not path.exists():
        return pd.DataFrame()
    df = pd.read_csv(path)
    parts = df["tag"].str.split(";", expand=True)
    df["suite"], df["label"], df["rep"] = parts[0], parts[1], parts[2].astype(int)
    if parts.shape[1] > 3:
        df["lead"] = parts[3]
    df["cores"] = df["ranks"] * df["threads_per_rank"]
    df["time"] = df["max_time_s"]  # wall time = slowest rank
    return df


def stats(g: pd.DataFrame) -> pd.Series:
    t = g["time"].to_numpy()
    q1, q3 = np.percentile(t, [25, 75])
    return pd.Series(
        {
            "n": len(t),
            "median_s": np.median(t),
            "min_s": t.min(),
            "max_s": t.max(),
            "range_s": t.max() - t.min(),
            "iqr_s": q3 - q1,
            "mups": float(g["megaupdates_per_s"].median()),
            "cores": int(g["cores"].iloc[0]),
            "ranks": int(g["ranks"].iloc[0]),
            "threads": int(g["threads_per_rank"].iloc[0]),
        }
    )


def strong_table(df: pd.DataFrame) -> pd.DataFrame:
    tab = df.groupby("label", sort=False).apply(stats, include_groups=False)
    base = tab.loc["mpi_P1_T1", "median_s"]
    tab["speedup"] = base / tab["median_s"]
    tab["efficiency_pct"] = 100.0 * tab["speedup"] / tab["cores"]
    return tab


def weak_table(df: pd.DataFrame) -> pd.DataFrame:
    tab = df.groupby("label", sort=False).apply(stats, include_groups=False)
    base = tab.loc["weak_P1", "median_s"]
    tab["weak_eff_pct"] = 100.0 * base / tab["median_s"]
    return tab


def paired_bootstrap(delta: np.ndarray):
    meds = np.median(RNG.choice(delta, size=(BOOT, len(delta)), replace=True), axis=1)
    return np.percentile(meds, [2.5, 97.5])


def overlap_result(df: pd.DataFrame) -> dict:
    ov = df[df["label"] == "overlap_true"].set_index("rep")["time"]
    nov = df[df["label"] == "overlap_false"].set_index("rep")["time"]
    pairs = pd.concat([ov.rename("ov"), nov.rename("nov")], axis=1).dropna()
    delta = (pairs["ov"] - pairs["nov"]).to_numpy()
    lo, hi = paired_bootstrap(delta)
    med = float(np.median(delta))
    improved = bool(med < 0 and hi < 0)
    worse = bool(med > 0 and lo > 0)
    return {
        "pairs": int(len(delta)),
        "median_overlap_s": float(pairs["ov"].median()),
        "median_nonoverlap_s": float(pairs["nov"].median()),
        "median_delta_s": med,
        "median_delta_pct": 100.0 * med / float(pairs["nov"].median()),
        "ci95_lo_s": float(lo),
        "ci95_hi_s": float(hi),
        "n_overlap_faster": int((delta < 0).sum()),
        "verdict": "IMPROVEMENT" if improved else ("OVERLAP SLOWER" if worse else "NO MEASURABLE DIFFERENCE"),
    }


def plot_strong(tab: pd.DataFrame, title: str, stem: str):
    mpi = tab.loc[[i for i in tab.index if i.startswith("mpi")]]
    omp = tab.loc[[i for i in tab.index if i.startswith("omp") or i == "mpi_P1_T1"]]
    hyb = tab.loc[["hybrid_P2_T2"]] if "hybrid_P2_T2" in tab.index else None
    fig, ax = plt.subplots(1, 3, figsize=(14, 4))
    for series, name, mk in ((mpi, "Pure MPI (P ranks, T=1)", "o-"), (omp, "Pure OpenMP (P=1, T threads)", "s-")):
        ax[0].plot(series["cores"], series["median_s"], mk, label=name)
        ax[1].plot(series["cores"], series["speedup"], mk, label=name)
        ax[2].plot(series["cores"], series["efficiency_pct"], mk, label=name)
    if hyb is not None:
        for a, col in zip(ax, ("median_s", "speedup", "efficiency_pct")):
            a.plot(hyb["cores"], hyb[col], "D", ms=9, label="Hybrid (P=2, T=2)")
    cores = np.array([1, 2, 4])
    ax[1].plot(cores, cores, "k--", lw=1, label="Ideal")
    ax[2].axhline(100, color="k", ls="--", lw=1)
    for a, yl in zip(ax, ("Median wall time (s)", "Speedup S(p)", "Efficiency E(p) (%)")):
        a.set_xlabel("Cores (P x T)")
        a.set_ylabel(yl)
        a.set_xticks(cores)
        a.grid(alpha=0.3)
    ax[0].legend(fontsize=8)
    fig.suptitle(title)
    fig.tight_layout()
    fig.savefig(FIG / f"{stem}.png", dpi=150)
    plt.close(fig)


def plot_weak(tabs: dict):
    fig, ax = plt.subplots(1, 2, figsize=(10, 4))
    for name, tab in tabs.items():
        ax[0].plot(tab["cores"], tab["median_s"], "o-", label=name)
        ax[1].plot(tab["cores"], tab["weak_eff_pct"], "o-", label=name)
    ax[1].axhline(100, color="k", ls="--", lw=1)
    ax[0].set_ylabel("Median wall time (s)")
    ax[1].set_ylabel("Weak-scaling efficiency (%)")
    for a in ax:
        a.set_xlabel("Cores (= MPI ranks)")
        a.set_xticks([1, 2, 4])
        a.grid(alpha=0.3)
    ax[0].legend()
    fig.suptitle("Weak scaling (constant work per core)")
    fig.tight_layout()
    fig.savefig(FIG / "g5_weak_scaling.png", dpi=150)
    plt.close(fig)


def plot_overlap(results: dict, dfs: dict):
    fig, ax = plt.subplots(1, len(dfs), figsize=(5 * len(dfs), 4), squeeze=False)
    for a, (name, df) in zip(ax[0], dfs.items()):
        for lab, c in (("overlap_true", "tab:blue"), ("overlap_false", "tab:orange")):
            s = df[df["label"] == lab].sort_values("rep")
            a.plot(s["rep"], s["time"], "o-", color=c, label=lab.replace("overlap_", "overlap="))
        r = results[name]
        a.set_title(f"{name}: delta={r['median_delta_s']*1e3:+.1f} ms ({r['median_delta_pct']:+.2f}%)\n"
                    f"95% CI [{r['ci95_lo_s']*1e3:+.1f}, {r['ci95_hi_s']*1e3:+.1f}] ms - {r['verdict']}", fontsize=9)
        a.set_xlabel("Pair index")
        a.set_ylabel("Wall time (s)")
        a.grid(alpha=0.3)
        a.legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(FIG / "g5_overlap.png", dpi=150)
    plt.close(fig)


def main():
    summary = {}
    out_lines = []
    for suite, title, stem in (("A", "2D strong scaling 4096x4096, 100 steps", "g5_strong_2d"),
                               ("B", "3D strong scaling 512^3, 50 steps", "g5_strong_3d")):
        df = load(suite)
        if df.empty:
            continue
        tab = strong_table(df)
        tab.to_csv(RES / f"g5_summary_{suite}.csv")
        plot_strong(tab, title, stem)
        summary[suite] = tab.reset_index().to_dict(orient="records")
        out_lines += [f"\n### Suite {suite}: {title}\n", tab[["cores", "median_s", "range_s", "iqr_s", "speedup", "efficiency_pct", "mups"]].round(4).to_markdown()]

    weak = {}
    for suite, name in (("C", "2D weak (2048^2/core)"), ("D", "3D weak (256^3/core)")):
        df = load(suite)
        if df.empty:
            continue
        tab = weak_table(df)
        tab.to_csv(RES / f"g5_summary_{suite}.csv")
        weak[name] = tab
        summary[suite] = tab.reset_index().to_dict(orient="records")
        out_lines += [f"\n### Suite {suite}: {name}\n", tab[["cores", "median_s", "range_s", "iqr_s", "weak_eff_pct"]].round(4).to_markdown()]
    if weak:
        plot_weak(weak)

    ov_res, ov_dfs = {}, {}
    for suite, name in (("E2D", "2D 4096^2"), ("E3D", "3D 512^3")):
        df = load(suite)
        if df.empty:
            continue
        ov_dfs[name] = df
        ov_res[name] = overlap_result(df)
    if ov_res:
        plot_overlap(ov_res, ov_dfs)
        summary["overlap"] = ov_res
        out_lines.append("\n### Suite E: overlap vs non-overlap (P=4, interleaved pairs)\n")
        out_lines.append(pd.DataFrame(ov_res).T.round(5).to_markdown())

    (RES / "g5_summary.json").write_text(json.dumps(summary, indent=2, default=float))
    (RES / "g5_summary.md").write_text("\n".join(out_lines) + "\n")
    print("\n".join(out_lines))


if __name__ == "__main__":
    main()
