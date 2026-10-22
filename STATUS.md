# Gate 2 (G2) Status Report: MPI Domain Decomposition & Halo Exchange

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Cartesian Domain Decomposition (`include/heat/decomposition.hpp`, `src/decomposition.cpp`)**:
  - Implemented 2D (`Decomposition2D`) and 3D (`Decomposition3D`) Cartesian process topology creation with `MPI_Dims_create` and `MPI_Cart_create`.
  - Remainder distribution logic correctly handles arbitrary grid sizes ($N$) across non-uniform rank grids ($P$):
    $$\text{local\_n} = \frac{N}{P} + (r < (N \pmod P) ? 1 : 0)$$
  - Neighbor discovery and boundary topology configured via `MPI_Cart_shift`, correctly establishing `MPI_PROC_NULL` on external physical domain boundaries.
- **Non-blocking Halo Exchange (`include/heat/halo.hpp`, `src/halo.cpp`)**:
  - Contiguous pack/unpack buffers for non-contiguous face boundaries (`send_left`, `send_right`, `send_bottom`, `send_top`, `send_back`, `send_front`).
  - Asynchronous communication posted with `MPI_Irecv` and `MPI_Isend`, completed with `MPI_Waitall` (ADR-0004).
  - Explicit physical boundary handling for `MPI_PROC_NULL` neighbors (Dirichlet and Neumann boundary evaluations).
- **Compute-Communication Overlap (`include/heat/mpi_solver.hpp`, `src/mpi_solver.cpp`)**:
  - Implemented both `stepOverlap` and `stepNoOverlap` paths:
    - *Overlap ON*: Start halo non-blocking `MPI_Irecv`/`MPI_Isend` $\implies$ immediately update interior cells $\implies$ wait for halo completion with `MPI_Waitall` $\implies$ update outer boundary shell.
    - *Overlap OFF*: Synchronously exchange all halos $\implies$ compute across whole subdomain in one pass.
  - Gather routines (`gather2D`, `gather3D`) reconstruct global domain onto Rank 0 for rigorous equivalence testing and snapshot output.
- **Dedicated MPI Test Suite (`tests/test_mpi.cpp`)**:
  - **Ghost-cell neighbor encoding test**: Each rank encodes its global coordinate `(gx * 10000 + gy)` into its interior. After exchange, every ghost cell is verified against the exact neighbor cell value across the Cartesian mesh.
  - **Serial vs. Parallel equivalence**: Ran across 1, 2, 3, 4, 6, and 8 ranks on prime and uneven grid resolutions ($67 \times 53$ in 2D, $37 \times 29 \times 31$ in 3D), testing both overlap on and off against the G1 serial solver.

## 3. What Was Verified & How

### A. Ghost-Cell Encoding Verification Test Output
Ran on uneven dimensions ($47 \times 31$):
```text
[ RUN      ] MpiHaloTest.GhostCellsMatchNeighborEncoding2D
[       OK ] MpiHaloTest.GhostCellsMatchNeighborEncoding2D (0 ms)
```
Every ghost cell holds the exact neighbor interior value across all ranks.

### B. Parallel Equals Serial Numerical Equivalence (Ranks 1 to 8)
All parallel runs match the G1 serial solver bit-for-bit or within machine precision (max difference $= 0$ or $< 10^{-12}$):

| Rank Count ($P$) | Process Grid ($P_x \times P_y$) | Test Case | Grid Size | Max Diff vs Serial | Result |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | $1 \times 1$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |
| **2** | $2 \times 1$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |
| **3** | $3 \times 1$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |
| **4** | $2 \times 2$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |
| **6** | $3 \times 2$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |
| **8** | $4 \times 2$ | 2D Uneven / Overlap / 3D Uneven | $67\times 53$, $64^2$, $37\times 29\times 31$ | `0.0` | **PASSED** |

### C. Toolchain & Leak Sanitizer Outputs
1. **Toolchain Documentation**:
   - Environment updated with `libstdc++-16-dev` (version 16-20260322-1ubuntu1) to align Clang 21 with GCC 15/16 C++ standard library headers. Recorded in README and STATUS.
2. **Leak Sanitizer Explicit Exit Codes**:
   ```bash
   python3 -c "import subprocess, os; env=dict(os.environ, ASAN_OPTIONS='detect_leaks=1'); ..."
   ```
   - `test_config returncode: 0`
   - `test_serial_solver returncode: 0`
3. **ASan & UBSan MPI Test Suite**:
   - OpenMPI MCA patcher set to `OMPI_MCA_patcher=^overwrite` (preventing ASan memory conflict in `mca_base_patcher_patch_apply_binary`).
   - Standard LSan suppression file `tests/lsan_mpi.supp` applied for OpenMPI internal global one-time setup structures (ADR-0005).
   - Ranks 1, 2, 3, 4 passed cleanly under ASan + LSan + UBSan with 0 project leaks.
4. **Native Clang-Tidy**:
   ```bash
   clang-tidy -p build/debug src/*.cpp tests/*.cpp
   ```
   - Clean exit (exit code 0), 0 diagnostics in project code across all 11 translation units.

## 4. Architecture Decision Records (ADRs) Recorded
- `docs/adr/0004-halo-exchange-strategy.md`: Justified contiguous pack/unpack buffers for non-blocking halo exchanges and overlap decoupling.
- `docs/adr/0005-sanitizer-mpi-suppressions.md`: Documented OpenMPI internal MCA runtime behavior under AddressSanitizer and LeakSanitizer.

## 5. Next Gate (G3)
- **Gate 3**: OpenMP multi-threading integration, NUMA first-touch initialization, thread affinity (`OMP_PROC_BIND`, `OMP_PLACES`), verification of `MPI_THREAD_FUNNELED`, programmatic metadata capture (CPU model, cores, cache, RAM, MPI/OpenMP versions), and initial benchmark driver producing raw CSV output.
