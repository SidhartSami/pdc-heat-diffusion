# Architecture & Design Overview

## 1. Domain & Memory Layout
To maximize spatial cache locality and enable effective compiler vectorization:
- 2D grid: Contiguous 1D buffer indexed via `(y * stride_x + x)`.
- 3D grid: Contiguous 1D buffer indexed via `(z * stride_y * stride_x + y * stride_x + x)`.
- A 1-cell halo (ghost layer) surrounds all active domain faces for local subdomains.
- Pointer swapping between double-buffers (`u_old` and `u_new`) avoids memory allocations during time marching.

## 2. MPI Cartesian Decomposition
- Domain decomposition uses `MPI_Cart_create` to organize ranks into a logical Cartesian grid.
- Uneven grid dimensions are distributed with remainder handling:
  $$\text{local\_n} = \frac{N}{P} + (r < (N \pmod P) ? 1 : 0)$$
- Each rank determines its Cartesian neighbours via `MPI_Cart_shift`.

## 3. Halo Exchange & Overlap Strategy
- Non-blocking exchanges (`MPI_Irecv` and `MPI_Isend`) are posted at the beginning of each step.
- When **Overlap Mode** is active:
  1. Post `MPI_Irecv` for all halo faces.
  2. Post `MPI_Isend` packing/transmitting boundary slices.
  3. Immediately update the **interior domain** (cells independent of ghost cells).
  4. Wait for halo exchanges to complete (`MPI_Waitall`).
  5. Update the **boundary shell** (cells touching the halo).
- When Overlap Mode is inactive:
  1. Post halo exchanges and wait immediately (`MPI_Waitall`).
  2. Compute across the entire subdomain in a single loop traversal.

## 4. OpenMP Threading & NUMA Strategy
- Work is partitioned across threads using `#pragma omp parallel for collapse(2)` or `#pragma omp parallel for schedule(static)`.
- Allocations are initialized in parallel with the same scheduling policy (First-Touch) to ensure memory pages map to the local NUMA domain of the executing thread.

## 5. Numerical Equivalence & Bit-Identical Precision
Because the FTCS stencil is an explicit finite-difference formula, updating cell $(i, j, k)$ in time step $n+1$ depends solely on the values at step $n$ of the cell and its immediate orthogonal neighbors:
$$u^{n+1}_{i,j,k} = u^n_{i,j,k} + \Delta t \left( \frac{\alpha}{\Delta x^2} (u^n_{i+1} - 2u^n_i + u^n_{i-1}) + \dots \right)$$
In both the serial solver and the distributed MPI solver:
1. Double-buffering prevents in-place overwrite hazards.
2. Arithmetic operations are executed in identical sequence with identical IEEE 754 double-precision floating point rounding per cell. In both synchronous and overlapped modes, the domain update is decomposed into disjoint cell sets (interior vs. boundary shell in overlap mode) without altering the algebraic operations or operand evaluation order for any individual cell.
3. As long as boundary halo cells received via MPI match the serial grid neighbor cells to the last bit, the computed interior and boundary update evaluates to exact bit-identical values (`Max Diff: 0.0`). This is tested across all rank counts $P \in \{1, 2, 3, 4, 6, 8\}$ and confirmed in unit testing.

### Floating-Point Associativity and G3 Tolerance Planning
While the local stencil evaluation per cell is bit-identical, global reductions (such as calculating the domain's total energy $\sum u_{i,j,k} \Delta V$ or global max norm) will exhibit non-associative floating-point summation drift when parallelized under OpenMP (`#pragma omp parallel for reduction(+:...))`. OpenMP distributes loop chunks dynamically or across variable numbers of threads, changing the accumulation order of summation trees.

Therefore, for G3 hybrid execution:
- **Per-cell grid equivalence** (`MPI + OpenMP` stencil update vs. serial reference) will remain strictly bit-identical (`Max Diff: 0.0` or $\le 10^{-14}$) because each cell's arithmetic stencil uses an invariant operand order.
- **Global energy / norm reductions** parallelized across threads will adopt an explicit tolerance of $\varepsilon = 10^{-12}$ to account for standard OpenMP tree summation reordering.
