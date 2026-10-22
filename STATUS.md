# Gate 1 (G1) Status Report: Serial 2D & 3D FTCS Solver

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Core Stencil Grid & Data Structures**:
  - `include/heat/grid.hpp`: Implemented `Grid2D` and `Grid3D` continuous single-allocation flat vectors (`std::vector<double>`) with explicit 1-cell halo boundaries on all sides. Fast row-major coordinate index mapping and zero-copy `swap` support for double buffering.
- **Physical Boundaries & Initial Conditions**:
  - `include/heat/boundary.hpp` and `src/boundary.cpp`:
    - Initial conditions: Decaying Sine modes, Gaussian hot-spot, and step function.
    - Boundary conditions: Dirichlet (exact physical values matching analytic modes at boundaries) and Neumann (zero-flux ghost cell assignment $u_0 = u_1$).
    - Energy calculation: Discrete domain integration for 2D and 3D grids.
- **Explicit FTCS Serial Solver Engine**:
  - `include/heat/solver.hpp` and `src/solver.cpp`:
    - 5-point central-difference stencil in 2D.
    - 7-point central-difference stencil in 3D.
    - Double-buffering pointer swapping (`u_old.swap(u_new)`).
    - Hard stability validation guard in solver constructor (`throw std::runtime_error` if $\Delta t > \Delta t_{\max}$).
- **Comprehensive Verification Suite**:
  - `tests/test_serial_solver.cpp`:
    - 1D, 2D, and 3D analytic sine decay verification.
    - Empirical spatial convergence order ($O(\Delta x^2)$) on refined grids ($N=32, 64, 128$).
    - Neumann energy conservation test (verifying $\int_\Omega u\,d\Omega = \text{const}$ within machine precision).
    - Solver constructor stability rejection test.

## 3. What Was Verified & How
- **End-to-End Application Exit Codes**:
  - `small_2d.cfg`: Ran cleanly to completion with exit code `0`.
  - `unstable_2d.cfg`: Instantly halted with exit code `2` with the explicit CFL violation diagnostic.
- **Analytic Solutions**:
  - 1D Sine Decay: Matched exact solution $e^{-\pi^2 \alpha t} \sin(\pi x)$ within $10^{-12}$.
  - 2D Sine Decay: Maximum pointwise error after 50 steps on $64 \times 64$ is $< 10^{-3}$.
  - 3D Sine Decay: Maximum pointwise error after 30 steps on $32^3$ is $< 2 \times 10^{-3}$.
- **Empirical Convergence Order ($O(\Delta x^2)$)**:
  - Computed spatial error $L_2$ norm across grid refinements with fixed Courant/diffusion number $r = 0.15$:
    | Resolution ($N$) | Time Step ($\Delta t$) | $L_2$ Error Norm | Error Reduction Ratio | Empirical Order ($p$) |
    | :--- | :--- | :--- | :--- | :--- |
    | **$32 \times 32$** | $1.4648 \times 10^{-4}$ | $6.37960 \times 10^{-6}$ | — | — |
    | **$64 \times 64$** | $3.6621 \times 10^{-5}$ | $1.53776 \times 10^{-6}$ | **4.1486** | **2.05** |
    | **$128 \times 128$** | $9.1553 \times 10^{-6}$ | $3.87848 \times 10^{-7}$ | **3.9649** | **1.99** |
  - *Observation*: The reduction ratio closely clusters around $4.0\times$ (theoretical $2^2 = 4$), confirming clean second-order spatial convergence.
- **Neumann Energy Conservation**:
  - Total energy initially: $\approx 0.0314159$.
  - Total energy after 100 FTCS diffusion steps: exactly identical within numerical precision (difference $< 10^{-12}$).
- **Sanitizers & Tooling Cleanliness**:
  - Debug preset: 9/9 tests passed.
  - Release preset: 9/9 tests passed.
  - ASan + UBSan preset: 9/9 tests passed cleanly without any memory leaks or undefined behavior.
  - `pre-commit run --all-files`: Passed all hooks (clang-format, whitespace, yaml, eof).
  - `clang-tidy`: Checked across all `src/*.cpp` and `include/heat/*.hpp`.

## 4. Architecture Decisions & Notes
- Double buffering is encapsulated cleanly using `std::vector::swap` which performs $O(1)$ pointer exchanges without buffer reallocations.
- Large 3D grid memory footprint: `configs/large_3d.cfg` ($512^3$) requires $\approx 2.15$ GB RAM for double buffers; documented in config header.

## 5. What Is Open / Next Gate (G2)
- **Gate 2**: MPI 2D and 3D Cartesian domain decomposition (`MPI_Cart_create`, `MPI_Cart_shift`), non-blocking halo exchange (`MPI_Isend`, `MPI_Irecv`), boundary/interior decomposition, and serial-parallel equivalence across 1 to 8 ranks including uneven grids.
