#!/usr/bin/env bash
# Headline run at the exact problem size named in the course proposal:
# 2D, 8192 x 8192 doubles, 5000 time steps. One run per configuration (each takes minutes), so
# this is a demonstration of the sequential-vs-parallel gap, not a statistical benchmark.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
OUT=results/proposal_size.csv
rm -f "${OUT}"
export OMP_PLACES=cores OMP_PROC_BIND=close BENCHMARK_CSV_FILE="${OUT}"
for cfg in "1 1 0" "4 1 0,2,4,6" "2 2 0,2,4,6"; do
    read -r P T cpus <<< "${cfg}"
    export OMP_NUM_THREADS="${T}" BENCHMARK_TAG="proposal;P${P}_T${T};1"
    echo "P=${P} T=${T} ..."
    taskset -c "${cpus}" mpirun --map-by "slot:PE=${T}" --bind-to core -np "${P}" \
        ./build/release/heat_solver --dim=2 --nx=8192 --ny=8192 --steps=5000 --dt=auto \
        --overlap=true | grep "Max Time"
done
