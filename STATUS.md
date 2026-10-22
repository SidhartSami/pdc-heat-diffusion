# Gate 1 (G1) Status Report: Serial 2D & 3D FTCS Solver

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Grid Data Structures**:
  - `include/heat/grid.hpp`: Implemented `Grid1D`, `Grid2D`, and `Grid3D` storing contiguous flat vectors (`std::vector<double>`) with explicit 1-cell halo padding on each boundary. Zero-overhead coordinate indexing and $O(1)$ pointer swapping (`grid_a.swap(grid_b)`).
- **Physical Boundary & Initial Conditions**:
  - `include/heat/boundary.hpp` & `src/boundary.cpp`:
    - Initial conditions: Decaying Sine modes (1D, 2D, 3D), Gaussian hot spot, and step function.
    - Boundary conditions: Dirichlet (exact time-dependent analytical boundary value enforcement) and Neumann (zero-flux ghost cell assignment).
    - Energy calculation: Discrete domain integration for 2D and 3D grids.
- **FTCS Serial Solver**:
  - `include/heat/solver.hpp` & `src/solver.cpp`:
    - 5-point explicit stencil in 2D.
    - 7-point explicit stencil in 3D.
    - Double-buffering pointer swapping (`u_old.swap(u_new)`).
    - Enforced stability validation guard in solver constructor (`throw std::runtime_error` if $\Delta t > \Delta t_{\max}$).
- **Comprehensive Verification Suite (`tests/test_serial_solver.cpp`)**:
  - 1D, 2D, and 3D analytic sine decay verification.
  - Full-scheme convergence order testing ($O(\Delta x^2)$) on refined grids in both 2D and 3D.
  - Neumann energy conservation test with exact printouts of initial energy, final energy, and difference.
  - Leak detection verified with `ASAN_OPTIONS=detect_leaks=1`.

## 3. What Was Verified & How

### A. Discrete $L_2$ Error Definition
The discrete $L_2$ norm is computed over interior physical cells excluding ghost layers:
$$\|e\|_{L_2} = \sqrt{\frac{1}{N^d} \sum_{i \in \text{interior}} (u_{\text{numeric}}(i) - u_{\text{exact}}(i, t_{\text{final}}))^2}$$
where $u_{\text{exact}}(x, y, z, t) = \sin(\pi x)\sin(\pi y)\sin(\pi z) e^{-d \pi^2 \alpha t}$.

### B. Convergence Rates
Under proportional refinement with fixed Courant number $r = \frac{\alpha \Delta t}{\Delta x^2}$ ($r = 0.15$ in 2D, $r = 0.10$ in 3D), measuring the combined second-order accuracy of the explicit FTCS scheme:

#### 1. 2D Convergence ($r = 0.15, \alpha = 1.0$)
| Resolution ($N \times N$) | $\Delta x$ | $\Delta t$ | $L_2$ Error Norm | Error Reduction Ratio | Empirical Order ($p = \log_2 \text{ratio}$) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **$32 \times 32$** | $3.1250 \times 10^{-2}$ | $1.4648 \times 10^{-4}$ | $6.37960 \times 10^{-6}$ | — | — |
| **$64 \times 64$** | $1.5625 \times 10^{-2}$ | $3.6621 \times 10^{-5}$ | $1.53776 \times 10^{-6}$ | **4.1486** | **2.05** |
| **$128 \times 128$** | $7.8125 \times 10^{-3}$ | $9.1553 \times 10^{-6}$ | $3.87850 \times 10^{-7}$ | **3.9649** | **1.99** |

#### 2. 3D Convergence ($r = 0.10, \alpha = 1.0$)
| Resolution ($N^3$) | $\Delta x$ | $\Delta t$ | $L_2$ Error Norm | Error Reduction Ratio | Empirical Order ($p = \log_2 \text{ratio}$) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **$24^3$** | $4.1667 \times 10^{-2}$ | $1.7361 \times 10^{-4}$ | $1.21020 \times 10^{-5}$ | — | — |
| **$48^3$** | $2.0833 \times 10^{-2}$ | $4.3403 \times 10^{-5}$ | $2.89885 \times 10^{-6}$ | **4.1748** | **2.06** |
| **$96^3$** | $1.0417 \times 10^{-2}$ | $1.0851 \times 10^{-5}$ | $7.24440 \times 10^{-7}$ | **4.0015** | **2.00** |

Both 2D and 3D solvers converge at the theoretical rate $p \approx 2.0$.

### C. Neumann Energy Conservation
- **Grid**: $48 \times 48$, $\Delta t = 5 \times 10^{-5}$, $100$ steps, zero-flux boundary conditions.
- **Initial Total Energy**: `0.06283178431486759`
- **Final Total Energy**:   `0.06283178431486740`
- **Absolute Difference**:  `1.9428902930940239e-16` (machine epsilon precision)
- **Mathematical origin of initial energy**:
  Initial Gaussian profile: $u_0(x, y) = \exp\left(-\frac{(x - 0.5)^2 + (y - 0.5)^2}{2\sigma^2}\right)$ with $2\sigma^2 = 0.02 \implies \sigma^2 = 0.01$.
  The continuous 2D Gaussian integral over $\mathbb{R}^2$ is:
  $$\iint_{-\infty}^{\infty} e^{-\frac{x^2+y^2}{2\sigma^2}} \, dx \, dy = 2\pi\sigma^2 = 2\pi(0.01) = \frac{2\pi}{100} \approx 0.06283185307...$$
  Since the Gaussian is well-contained within $[0, 1]^2$ ($5\sigma = 0.5$, decaying to $< 10^{-5}$ at boundaries), the discrete Riemann sum approximates $\frac{2\pi}{100} \approx 0.0628318$ to 5 significant digits.

### D. Test Presets & Sanitizers
- `ctest --preset debug`: **10/10 passed** (8.38s).
- `ctest --preset asan-ubsan`: **10/10 passed** (19.86s).
- Explicit leak detection verified: `ASAN_OPTIONS=detect_leaks=1` passed cleanly with 0 leaks reported.
- `pre-commit run --all-files`: 100% passed (trailing-whitespace, end-of-file-fixer, check-yaml, check-added-large-files, clang-format).
- `clang-tidy src/*.cpp`: Clean execution with exit code 0.

## 4. Architecture Decision Records (ADRs) Recorded
- `docs/adr/0003-convergence-methodology.md`: Documented full-scheme vs. pure spatial convergence verification methodology and discrete $L_2$ norm definition.

## 5. What Is Open / Next Gate (G2)
- **Gate 2**: Implementation of MPI 2D/3D Cartesian domain decomposition (`MPI_Cart_create`), non-blocking halo exchanges (`MPI_Isend` / `MPI_Irecv` with `MPI_Waitall`), explicit ghost-to-neighbor verification test, and parallel-vs-serial numerical equivalence across 1 to 8 ranks (including uneven process counts).
