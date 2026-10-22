# Project Proposal: Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

**Course**: Parallel & Distributed Computing (PDC)
**Reference**: Project #12 (Heat Diffusion / Stencil Computation)
**Model**: Hybrid Distributed-Shared Memory (MPI + OpenMP)
**Group Members**: [Group Member Placeholders]

---

## 1. Executive Summary & Problem Formulation
The goal of this project is to model, implement, optimize, and analyze a massively parallel numerical solver for transient thermal diffusion governed by the parabolic partial differential equation:
$$\frac{\partial u}{\partial t} = \alpha \nabla^2 u = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} + \left[\frac{\partial^2 u}{\partial z^2}\right] \right)$$
on a unit domain $\Omega \in [0, 1]^d$ ($d \in \{2, 3\}$), subject to specified boundary conditions (Dirichlet or zero-flux Neumann).

We use an explicit Forward-Time Central-Space (FTCS) finite difference discretization with a 5-point stencil in 2D and 7-point stencil in 3D. The numerical stability of the explicit scheme is strictly governed by the Courant-Friedrichs-Lewy (CFL) condition:
$$\Delta t \le \frac{1}{2 d \alpha \sum_{i=1}^d \frac{1}{\Delta x_i^2}}$$

## 2. Target Problem Sizes
- **2D Grids**: Baseline $1024 \times 1024$ scaling up to $4096 \times 4096$ (16.7M cells).
- **3D Grids**: Baseline $128^3$ scaling up to $512^3$ (134.2M cells, >1 GB per time-step buffer).

## 3. Parallel Architecture & Implementation Strategy
1. **Distributed Memory (MPI)**:
   - 2D/3D Cartesian topology decomposition (`MPI_Cart_create`, `MPI_Cart_shift`) to minimize surface-to-volume communication ratios.
   - Non-blocking point-to-point halo exchange (`MPI_Isend` / `MPI_Irecv`).
   - Compute-communication overlap separating interior grid updates from halo-dependent boundary shells.
2. **Shared Memory (OpenMP)**:
   - Multithreaded loop execution over subdomain slices (`#pragma omp parallel for`).
   - First-touch memory allocation policy ensuring optimal NUMA page placement.
   - Thread affinity and core binding (`OMP_PROC_BIND=close`, `OMP_PLACES=cores`).
3. **Hardware-Aware Optimizations**:
   - Cache blocking / spatial loop tiling for 3D grid traversals.
   - Explicit SIMD alignment and compiler auto-vectorization verification.

## 4. Work Plan & Milestone Gates
- **G0**: Repository scaffold, CMake presets, CI pipeline, config parser.
- **G1**: Serial solver, analytical sine-decay verification, convergence rate analysis ($O(\Delta x^2)$).
- **G2**: MPI domain decomposition, halo exchange, serial-parallel equivalence up to 8 ranks.
- **G3**: OpenMP multi-threading integration, hybrid rank/thread sweeps, metadata logging.
- **G4**: Performance optimizations (cache tiling, SIMD, overlap benchmarking).
- **G5**: Comprehensive scaling benchmarks (strong, weak, roofline analysis).
- **G6**: ParaView visualization (VTI/pvti), animations, checkpoint/restart, final report and viva prep.
