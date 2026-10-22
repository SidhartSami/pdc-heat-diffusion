#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${REPO_ROOT}"

OUTPUT_CSV="${REPO_ROOT}/bench/benchmark_g3.csv"
rm -f "${OUTPUT_CSV}"

export BENCHMARK_CSV_FILE="${OUTPUT_CSV}"

# Configure OpenMP 4.0 portable pinning to P-cores (1 thread per core: CPUs 0, 2, 4, 6)
export OMP_PLACES="{0},{2},{4},{6}"
export OMP_PROC_BIND=close

CONFIG_FILE="${REPO_ROOT}/configs/small_2d.cfg"

echo "=========================================================="
echo "Starting Gate 3 Hybrid MPI+OpenMP Benchmarks (5 repetitions)"
echo "Config: ${CONFIG_FILE}"
echo "CSV Output: ${OUTPUT_CSV}"
echo "=========================================================="

# Configurations: (MPI_RANKS, OMP_THREADS)
# We test 4 core allocations on the 4 P-cores:
# (1 rank x 1 thread), (1 rank x 2 threads), (1 rank x 4 threads),
# (2 ranks x 1 thread), (2 ranks x 2 threads), (4 ranks x 1 thread)
CONFIGURATIONS=(
    "1 1"
    "1 2"
    "1 4"
    "2 1"
    "2 2"
    "4 1"
)

for cfg in "${CONFIGURATIONS[@]}"; do
    read -r np nth <<< "${cfg}"
    echo "----------------------------------------------------------"
    echo "Running configuration: MPI ranks = ${np}, OpenMP threads = ${nth}"
    export OMP_NUM_THREADS="${nth}"

    for rep in {1..5}; do
        echo -n "  Repetition ${rep}/5: "
        mpirun --oversubscribe -np "${np}" ./build/release/heat_solver --config "${CONFIG_FILE}" > /dev/null
        echo "done"
    done
done

echo "=========================================================="
echo "Benchmark runs complete. Calculating statistics..."
echo "=========================================================="

python3 "${REPO_ROOT}/bench/analyze_g3.py" "${OUTPUT_CSV}"
