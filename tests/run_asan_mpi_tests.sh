#!/usr/bin/env bash
set -e
cd /mnt/c/University/PDC/Project

export OMPI_MCA_patcher=^overwrite
export LSAN_OPTIONS="suppressions=/mnt/c/University/PDC/Project/tests/lsan_mpi.supp"
export ASAN_OPTIONS="detect_leaks=1"

for np in 1 2 3 4; do
    echo "=========================================================="
    echo ">>> RUNNING ASAN/UBSAN MPI TESTS WITH NP = ${np}"
    echo "=========================================================="
    mpirun -np ${np} ./build/asan-ubsan/tests/test_mpi
done
echo "ALL SANITIZER MPI TESTS PASSED CLEANLY!"
