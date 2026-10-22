#!/usr/bin/env python3
import sys
import csv
import math
from collections import defaultdict

def analyze(csv_file):
    with open(csv_file, mode="r", newline="") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    print(f"\nAnalyzing Benchmark Results from: {csv_file}\n")

    groups = defaultdict(list)
    for row in rows:
        key = (int(row["ranks"]), int(row["threads_per_rank"]))
        groups[key].append(row)

    header = f"{'Ranks':<6} {'Threads':<8} {'Total':<6} {'Runs':<5} {'Median (s)':<12} {'Min (s)':<12} {'Max (s)':<12} {'Spread (s)':<12} {'Throughput (MUps/s)':<20}"
    print(header)
    print("-" * len(header))

    summary_rows = []

    for (ranks, threads) in sorted(groups.keys()):
        group = groups[(ranks, threads)]
        times = [float(r["max_time_s"]) for r in group]
        times.sort()
        n = len(times)
        
        # Median
        if n % 2 == 1:
            median_time = times[n // 2]
        else:
            median_time = 0.5 * (times[n // 2 - 1] + times[n // 2])

        min_time = times[0]
        max_time = times[-1]
        spread = max_time - min_time

        dim = int(group[0]["dimension"])
        nx = int(group[0]["nx"])
        ny = int(group[0]["ny"])
        nz = int(group[0]["nz"])
        steps = int(group[0]["steps"])
        total_cells = nx * ny * (nz if dim == 3 else 1)
        mups_median = (total_cells * steps / median_time) / 1e6

        print(f"{ranks:<6} {threads:<8} {ranks*threads:<6} {n:<5} {median_time:<12.6f} {min_time:<12.6f} {max_time:<12.6f} {spread:<12.6f} {mups_median:<20.3f}")

        summary_rows.append({
            "ranks": ranks,
            "threads_per_rank": threads,
            "total_threads": ranks * threads,
            "runs": n,
            "median_time_s": median_time,
            "min_time_s": min_time,
            "max_time_s": max_time,
            "spread_s": spread,
            "megaupdates_per_s": mups_median
        })

    summary_csv = csv_file.replace(".csv", "_summary.csv")
    with open(summary_csv, mode="w", newline="") as f:
        fieldnames = ["ranks", "threads_per_rank", "total_threads", "runs", "median_time_s", "min_time_s", "max_time_s", "spread_s", "megaupdates_per_s"]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(summary_rows)

    print(f"\nSummary table saved to: {summary_csv}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: analyze_g3.py <benchmark_csv_path>")
        sys.exit(1)
    analyze(sys.argv[1])
