#!/usr/bin/env bash
# Live demo for the PDC project: correctness -> speedup -> visualization -> checkpoint/restart.
# Run inside WSL/Linux from anywhere:   ./demo.sh            (full, ~2 min)
#                                       ./demo.sh quick      (skips the larger timing run)
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
BIN=./build/release/heat_solver
QUICK="${1:-}"
hr() { printf '\n==================== %s ====================\n' "$1"; }

hr "0. Build (Release, GCC + OpenMPI + OpenMP)"
cmake --preset release > /dev/null
cmake --build --preset release -j"$(nproc)" 2>&1 | tail -2

hr "1. Correctness: parallel result == serial result (bit-identical)"
for np in 1 4 6; do
    echo "--- mpirun -np ${np}: serial-vs-parallel 2D/3D, overlap on/off, Neumann ---"
    mpirun --oversubscribe -np "${np}" ./build/release/tests/test_mpi \
        --gtest_filter='MpiSolverTest.*' 2>&1 | grep -E "Max Diff|Energy|PASSED|FAILED" || true
done

hr "2. Speedup: 2048x2048 grid, 100 steps, P-cores 0,2,4,6 (1 -> 2 -> 4 cores)"
export OMP_PLACES=cores OMP_PROC_BIND=close
for cfg in "1 1 0" "2 1 0,2" "4 1 0,2,4,6" "2 2 0,2,4,6"; do
    read -r P T cpus <<< "${cfg}"
    OMP_NUM_THREADS="${T}" taskset -c "${cpus}" mpirun --map-by "slot:PE=${T}" --bind-to core -np "${P}" \
        "${BIN}" --dim=2 --nx=2048 --ny=2048 --steps=100 --dt=auto --overlap=true \
        | awk -v P="${P}" -v T="${T}" '/Max Time/ {printf "  P=%d ranks x T=%d threads : %s s\n", P, T, $4}'
done
if [[ "${QUICK}" != "quick" ]]; then
    echo "(full strong/weak scaling results: results/g5/g5_summary.md, figures: analysis/figures/)"
fi

hr "3. Visualization: Gaussian heat spot diffusing, 4 MPI ranks -> ParaView-readable VTI/PVD"
rm -rf output/demo_viz
OMP_NUM_THREADS=1 taskset -c 0,2,4,6 mpirun --bind-to core -np 4 "${BIN}" --dim=2 --nx=256 --ny=256 --steps=3000 \
    --dt=auto --ic=gaussian --boundary=dirichlet --overlap=true --snapshot-freq=300 \
    --output=output/demo_viz | grep -E "Max Time|Throughput"
ls output/demo_viz | head -6
echo "..."
if python3 -c "import numpy, matplotlib" 2>/dev/null; then
    python3 analysis/visualize.py output/demo_viz
elif command -v python.exe > /dev/null && python.exe -c "import numpy, matplotlib" 2>/dev/null; then
    python.exe analysis/visualize.py output/demo_viz
else
    echo ">> numpy/matplotlib not found. On Windows run: python analysis/visualize.py output/demo_viz"
fi
echo "Open output/demo_viz/heat.pvd in ParaView (time-series) or view output/demo_viz/heat_animation.gif"

hr "4. Checkpoint / restart: interrupted run must equal uninterrupted run exactly"
rm -rf output/demo_full output/demo_restart output/demo_ckpt
common=(--dim=2 --nx=200 --ny=160 --dt=auto --ic=gaussian --overlap=true)
OMP_NUM_THREADS=1 taskset -c 0,2,4,6 mpirun --bind-to core -np 4 "${BIN}" "${common[@]}" --steps=600 \
    --snapshot-freq=600 --output=output/demo_full > /dev/null
OMP_NUM_THREADS=1 taskset -c 0,2,4,6 mpirun --bind-to core -np 4 "${BIN}" "${common[@]}" --steps=300 \
    --checkpoint-freq=300 --checkpoint-path=output/demo_ckpt/ckpt > /dev/null
echo "  run stopped at step 300 after writing a checkpoint (simulated interruption); restarting from it to step 600..."
OMP_NUM_THREADS=1 taskset -c 0,2,4,6 mpirun --bind-to core -np 4 "${BIN}" "${common[@]}" --steps=600 \
    --restart-from=output/demo_ckpt/ckpt --snapshot-freq=600 --output=output/demo_restart > /dev/null
ok=1
for r in 0 1 2 3; do
    cmp -s "output/demo_full/heat_000600_r${r}.vti" "output/demo_restart/heat_000600_r${r}.vti" || ok=0
done
if [[ "${ok}" == 1 ]]; then echo "  RESULT: restarted run is BYTE-IDENTICAL to the uninterrupted run"; else echo "  RESULT: MISMATCH"; exit 1; fi

hr "Done"
