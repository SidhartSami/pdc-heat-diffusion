#!/usr/bin/env bash
set -e
cd /mnt/c/University/PDC/Project

for np in 1 2 3 4 6 8; do
    echo "=========================================================="
    echo ">>> RUNNING MPI TESTS WITH NP = ${np}"
    echo "=========================================================="
    mpirun -np ${np} ./build/debug/tests/test_mpi
done
echo "ALL RANK COUNTS PASSED SUCCESSFULLY!"
