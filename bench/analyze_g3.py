#!/usr/bin/env python3
import sys
import csv
from collections import defaultdict

def analyze(csv_file):
    with open(csv_file, mode="r", newline="") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    print(f"\nAnalyzing Benchmark Results from: {csv_file}\n")

    groups = defaultdict(list)
    for row in rows:
        key = (int(row["dimension"]), int(row["nx"]), int(row["ny"]), int(row["ranks"]), int(row["threads_per_rank"]))
        groups[key].append(row)

    # Calculate baseline time (1 rank, 1 thread) per grid size
    baselines = {}
    best_single_rank = {}

    for (dim, nx, ny, ranks, threads), group in groups.items():
        grid_key = (dim, nx, ny)
        times = sorted([float(r["max_time_s"]) for r in group])
        n = len(times)
        med = times[n // 2] if n % 2 == 1 else 0.5 * (times[n // 2 - 1] + times[n // 2])
        
        if ranks == 1 and threads == 1:
            baselines[grid_key] = med
        if ranks == 1:
            if grid_key not in best_single_rank or med < best_single_rank[grid_key]:
                best_single_rank[grid_key] = med

    header = f"{'Grid':<10} {'Overlap':<8} {'Ranks':<6} {'Threads':<8} {'Cores':<6} {'Runs':<5} {'Median (s)':<11} {'Min (s)':<11} {'Max (s)':<11} {'Spread (s)':<11} {'Throughput(MUps/s)':<19} {'Speedup(1T)':<12} {'Speedup(Best1R)':<16} {'Efficiency':<10}"
    print(header)
    print("-" * len(header))

    summary_rows = []

    for (dim, nx, ny, ranks, threads) in sorted(groups.keys()):
        group = groups[(dim, nx, ny, ranks, threads)]
        times = sorted([float(r["max_time_s"]) for r in group])
        n = len(times)
        
        median_time = times[n // 2] if n % 2 == 1 else 0.5 * (times[n // 2 - 1] + times[n // 2])
        min_time = times[0]
        max_time = times[-1]
        spread = max_time - min_time

        nz = int(group[0]["nz"])
        steps = int(group[0]["steps"])
        overlap_flag = "ON" if int(group[0]["overlap"]) == 1 else "OFF"
        total_cells = nx * ny * (nz if dim == 3 else 1)
        mups_median = (total_cells * steps / median_time) / 1e6

        cores = ranks * threads
        grid_key = (dim, nx, ny)
        base_1t = baselines.get(grid_key, median_time)
        base_best_1r = best_single_rank.get(grid_key, median_time)

        speedup_1t = base_1t / median_time
        speedup_best_1r = base_best_1r / median_time
        efficiency = speedup_1t / cores

        grid_str = f"{nx}x{ny}"
        print(f"{grid_str:<10} {overlap_flag:<8} {ranks:<6} {threads:<8} {cores:<6} {n:<5} {median_time:<11.6f} {min_time:<11.6f} {max_time:<11.6f} {spread:<11.6f} {mups_median:<19.3f} {speedup_1t:<12.2f} {speedup_best_1r:<16.2f} {efficiency*100:<9.1f}%")

        summary_rows.append({
            "grid": grid_str,
            "overlap": overlap_flag,
            "ranks": ranks,
            "threads_per_rank": threads,
            "cores": cores,
            "runs": n,
            "median_time_s": median_time,
            "min_time_s": min_time,
            "max_time_s": max_time,
            "spread_s": spread,
            "megaupdates_per_s": mups_median,
            "speedup_vs_1t": speedup_1t,
            "speedup_vs_best_1rank": speedup_best_1r,
            "parallel_efficiency": efficiency,
            "omp_places": group[0]["omp_places"],
            "omp_proc_bind": group[0]["omp_proc_bind"]
        })

    summary_csv = csv_file.replace(".csv", "_summary.csv")
    with open(summary_csv, mode="w", newline="") as f:
        fieldnames = ["grid", "overlap", "ranks", "threads_per_rank", "cores", "runs", "median_time_s", "min_time_s", "max_time_s", "spread_s", "megaupdates_per_s", "speedup_vs_1t", "speedup_vs_best_1rank", "parallel_efficiency", "omp_places", "omp_proc_bind"]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(summary_rows)

    print(f"\nSummary table saved to: {summary_csv}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: analyze_g3.py <benchmark_csv_path>")
        sys.exit(1)
    analyze(sys.argv[1])
