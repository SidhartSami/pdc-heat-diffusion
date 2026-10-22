#!/usr/bin/env bash
set -e
cd /mnt/c/University/PDC/Project

export OMPI_MCA_patcher=^overwrite
export LSAN_OPTIONS="suppressions=/mnt/c/University/PDC/Project/tests/lsan_mpi.supp"
export ASAN_OPTIONS="detect_leaks=1"

for np in 1 2 3 4 6 8; do
    echo "=========================================================="
    echo ">>> RUNNING ASAN/UBSAN MPI TESTS WITH NP = ${np}"
    echo "=========================================================="
    mpirun -np ${np} ./build/asan-ubsan/tests/test_mpi
    echo "ASAN_EXIT_CODE_NP_${np}: $?"
done
echo "ALL SANITIZER MPI TESTS (1 TO 8 RANKS) PASSED CLEANLY!"
