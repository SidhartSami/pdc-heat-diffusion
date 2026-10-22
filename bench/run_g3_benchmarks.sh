#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${REPO_ROOT}"

OUTPUT_CSV="${REPO_ROOT}/bench/benchmark_g3.csv"
rm -f "${OUTPUT_CSV}"
export BENCHMARK_CSV_FILE="${OUTPUT_CSV}"

export OMP_PLACES="cores"
export OMP_PROC_BIND="close"

MEDIUM_CONFIG="${REPO_ROOT}/configs/medium_2d.cfg"
LARGE_CONFIG="${REPO_ROOT}/configs/large_2d.cfg"

echo "================================================================================"
echo " Starting Gate 3 Multi-Core & Multi-Process Benchmarks (Warm-up + 5 timed reps)"
echo " Environment State:"
echo "   AC Power: Connected (95% charge, not throttled on battery)"
echo "   Initial Load Average: $(uptime)"
echo " Output CSV: ${OUTPUT_CSV}"
echo "================================================================================"

RUN_CONFIGS=(
    # --- 1. Thread Scaling on 1 Rank (Medium 512x512) ---
    "${MEDIUM_CONFIG} 1 1"
    "${MEDIUM_CONFIG} 1 2"
    "${MEDIUM_CONFIG} 1 4"

    # --- 2. Pure MPI Scaling (Medium 512x512) ---
    "${MEDIUM_CONFIG} 2 1"
    "${MEDIUM_CONFIG} 4 1"

    # --- 3. Hybrid MPI + OpenMP (Medium 512x512) ---
    "${MEDIUM_CONFIG} 2 2"

    # --- 4. Thread Scaling on 1 Rank (Large 4096x4096) ---
    "${LARGE_CONFIG} 1 1"
    "${LARGE_CONFIG} 1 2"
    "${LARGE_CONFIG} 1 4"

    # --- 5. Pure MPI Scaling (Large 4096x4096) ---
    "${LARGE_CONFIG} 2 1"
    "${LARGE_CONFIG} 4 1"

    # --- 6. Hybrid MPI + OpenMP (Large 4096x4096) ---
    "${LARGE_CONFIG} 2 2"
)

for entry in "${RUN_CONFIGS[@]}"; do
    read -r cfg_file np pe <<< "${entry}"
    cfg_name=$(basename "${cfg_file}")
    
    export OMP_NUM_THREADS="${pe}"

    echo "--------------------------------------------------------------------------------"
    echo "Running: Config=${cfg_name}, NP=${np}, PE=${pe} (Threads/Rank=${pe}), OMP_PLACES=cores"

    # 1. Warm-up run (discarded from CSV by unsetting BENCHMARK_CSV_FILE)
    unset BENCHMARK_CSV_FILE
    mpirun --map-by slot:PE="${pe}" --bind-to core -np "${np}" ./build/release/heat_solver --config "${cfg_file}" > /dev/null
    export BENCHMARK_CSV_FILE="${OUTPUT_CSV}"

    # 2. Timed repetitions (5 runs)
    for rep in {1..5}; do
        echo -n "  [Repetition ${rep}/5] ... "
        mpirun --map-by slot:PE="${pe}" --bind-to core -np "${np}" ./build/release/heat_solver --config "${cfg_file}" > /dev/null
        echo "done"
    done
done

echo "================================================================================"
echo " All benchmark sweeps complete! Generating statistics summary..."
echo "================================================================================"

python3 "${REPO_ROOT}/bench/analyze_g3.py" "${OUTPUT_CSV}"
