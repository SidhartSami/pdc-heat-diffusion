#!/usr/bin/env bash
set -e

# Resolve repository root relative to this script's directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${REPO_ROOT}"

export OMPI_MCA_patcher=^overwrite
export LSAN_OPTIONS="suppressions=${REPO_ROOT}/tests/lsan_mpi.supp"
export ASAN_OPTIONS="detect_leaks=1"

for np in 1 2 3 4 6 8; do
    echo "=========================================================="
    echo ">>> RUNNING ASAN/UBSAN MPI TESTS WITH NP = ${np}"
    echo "=========================================================="
    mpirun -np ${np} ./build/asan-ubsan/tests/test_mpi
    echo "ASAN_EXIT_CODE_NP_${np}: $?"
done
echo "ALL SANITIZER MPI TESTS (1 TO 8 RANKS) PASSED CLEANLY!"
