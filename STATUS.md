# Gate 2 (G2) Status Report: MPI Domain Decomposition & Halo Exchange

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done & Verified

### A. Full MPI Test Suite Coverage (8 Comprehensive Tests)
1. `MpiHaloTest.GhostCellsMatchNeighborEncoding2D`: Verifies that after halo exchange across Cartesian topology, every 2D ghost cell matches the neighbor's coordinate-encoded interior cell exactly on uneven grids ($47 \times 31$).
2. `MpiHaloTest.GhostCellsMatchNeighborEncoding3D`: Verifies 3D face halo exchange (6 faces: left, right, bottom, top, back, front) with coordinate encoding on uneven grid ($29 \times 23 \times 19$).
3. `MpiSolverTest.ParallelEqualsSerial2DUneven`: Serial vs parallel equivalence on uneven grid ($67 \times 53$).
4. `MpiSolverTest.OverlapEqualsSerial2D`: Compute-communication overlap ON equivalence test ($64 \times 64$).
5. `MpiSolverTest.ParallelEqualsSerial3DUneven`: Serial vs parallel equivalence on uneven 3D grid ($37 \times 29 \times 31$).
6. `MpiSolverTest.OverlapEqualsSerial3D`: 3D compute-communication overlap ON equivalence test ($32 \times 32 \times 32$).
7. `MpiSolverTest.NeumannEnergyConservationMPI2D`: Neumann zero-flux boundary condition tested across multiple MPI ranks with `MPI_PROC_NULL` on external faces; verifies exact energy equivalence to serial Neumann solver.
8. `MpiMutationTest.BrokenHaloExchangeFails`: Deliberately mutates a received halo cell by $+999.0$ and verifies that the comparison against the true neighbor value fails (`EXPECT_NE`).

### B. Bit-Identical Precision Explanation (Design.md, Section 5)
In an explicit FTCS stencil with double buffering, cell $(i, j, k)$ at step $n+1$ depends solely on the values at step $n$. Because identical IEEE 754 arithmetic operations are applied in the exact same sequence with the same floating-point operands, the parallel result is bit-identical (`Max Diff: 0.0`) to the serial solver. The mutation test confirms that any transmission flaw or indexing error immediately produces non-zero differences.

### C. Test Output & Explicit Exit Codes for All Rank Counts (1, 2, 3, 4, 6, 8)
- `NP = 1`: Exit code `0`
- `NP = 2`: Exit code `0`
- `NP = 3`: Exit code `0`
- `NP = 4`: Exit code `0`
- `NP = 6`: Exit code `0`
- `NP = 8`: Exit code `0`

### D. AddressSanitizer & LeakSanitizer Verification (Ranks 1 to 8)
- Ran all MPI tests under `ASAN_OPTIONS=detect_leaks=1` for rank counts 1, 2, 3, 4, 6, and 8 via `tests/run_asan_mpi_tests.sh`.
- Scoped suppressions (`tests/lsan_mpi.supp`) target exclusively external OpenMPI libraries (`/libopen-pal.so`, `/libpmix.so`, `/libmpi.so`, `/libuct`, `/libhsa-runtime`).
- Tested leak detection sensitivity: deliberately injected `new double[1024]` in a project test. LSan intercepted and reported the leak immediately (`8192 bytes leaked in 1 allocation`).
- All ranks 1 through 8 passed ASan + LSan + UBSan with exit code `0`.

### E. Clang-Tidy & NOLINT Audit
- **NOLINT audit**: Grepped all project files (`include/heat/`, `src/`, `tests/`). **Zero `NOLINT` comments exist in any project code.**
- The 108 `NOLINT` comments reported earlier originate entirely from third-party GoogleTest internal headers fetched into `build/debug/_deps/googletest-src/`.
- Native `clang-tidy -p build/debug src/*.cpp tests/*.cpp` executed with **exit code 0** and zero diagnostics in project code.

### F. Toolchain Clarification
- Confirmed active compiler in `build/debug/CMakeCache.txt`: `CMAKE_CXX_COMPILER:FILEPATH=/usr/bin/c++`, which is `g++ 15.2.0 (Ubuntu 15.2.0-16ubuntu1)`.
- Updated documentation to cleanly state the single verified compiler toolchain.

## 3. Architecture Decision Records (ADRs) Recorded
- `docs/adr/0004-halo-exchange-strategy.md`: Contiguous pack/unpack buffers for non-blocking halo exchanges and overlap decoupling.
- `docs/adr/0005-sanitizer-mpi-suppressions.md`: OpenMPI internal MCA runtime behavior under AddressSanitizer and LeakSanitizer.

## 4. Next Gate (G3)
- **Gate 3**: OpenMP multi-threading integration, NUMA first-touch initialization, thread affinity (`OMP_PROC_BIND`, `OMP_PLACES`), verification of `MPI_THREAD_FUNNELED`, programmatic metadata capture (CPU model, cores, cache, RAM, MPI/OpenMP versions), and initial benchmark driver producing raw CSV output.
