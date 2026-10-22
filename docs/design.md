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
