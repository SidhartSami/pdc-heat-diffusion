# Gate 1 (G1) Status Report: Serial 2D & 3D FTCS Solver

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Grid Abstractions (`include/heat/grid.hpp`)**:
  - Implemented `Grid1D`, `Grid2D`, and `Grid3D` continuous single-allocation flat vectors (`std::vector<double>`) with explicit 1-cell halo boundaries on all sides. Fast coordinate index mapping and zero-copy `swap` support for double buffering.
- **Physical Boundary & Initial Conditions (`include/heat/boundary.hpp`, `src/boundary.cpp`)**:
  - Initial conditions: Decaying Sine modes (1D, 2D, 3D), Gaussian hot spot, and step function.
  - Boundary conditions: Dirichlet (exact time-dependent analytical boundary value enforcement) and Neumann (zero-flux ghost cell assignment).
  - Energy calculation: Discrete domain integration for 2D and 3D grids.
- **FTCS Serial Solver (`include/heat/solver.hpp`, `src/solver.cpp`)**:
  - 5-point explicit stencil in 2D.
  - 7-point explicit stencil in 3D.
  - Double-buffering pointer swapping (`u_old.swap(u_new)`).
  - Enforced stability validation guard in solver constructor (`throw std::runtime_error` if $\Delta t > \Delta t_{\max}$).
- **Comprehensive Verification Suite (`tests/test_serial_solver.cpp`)**:
  - 1D, 2D, and 3D analytic sine decay verification.
  - Full-scheme convergence order testing ($O(\Delta x^2)$) on refined grids in both 2D and 3D, evaluating the exact analytical solution at the exact physical time reached $t = \text{steps} \cdot \Delta t$.
  - Neumann energy conservation test with exact printouts of initial energy, final energy, and difference.
  - Full test suite verified leak-free with `ASAN_OPTIONS=detect_leaks=1`.

## 3. What Was Verified & How

### A. Clarification on Gaussian Energy Value
- In the initial G1 report, the initial energy was errantly stated as $\pi/100 \approx 0.0314159$. This was an arithmetic typo in the report text: $\iint_{\mathbb{R}^2} \exp(-r^2/(2\sigma^2)) = 2\pi\sigma^2$.
- Here $2\sigma^2 = 0.02 \implies \sigma^2 = 0.01$, yielding $2\pi(0.01) = \frac{2\pi}{100} \approx 0.062831853...$
- The solver output (`0.06283178431486759`) matches $\frac{2\pi}{100}$ to within $7 \times 10^{-8}$.
- The discrete energy difference over 100 FTCS time steps with Neumann zero-flux conditions is `1.9428902930940239e-16`.

### B. Convergence Rates & Exact Time Evaluation
To prevent phase or decay mismatch from integer step rounding, the exact analytical solution is evaluated at the exact final time $t_{\text{final}} = \text{steps} \cdot \Delta t$:

#### 1. 2D Convergence ($r = 0.15, \alpha = 1.0$)
| Resolution ($N \times N$) | $\Delta x$ | $\Delta t$ | Steps | $t_{\text{final}}$ | $L_2$ Error Norm | Reduction Ratio | Empirical Order ($p$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **$32 \times 32$** | $3.1250 \times 10^{-2}$ | $1.4648 \times 10^{-4}$ | 7 | $0.0010254$ | $6.37960 \times 10^{-6}$ | — | — |
| **$64 \times 64$** | $1.5625 \times 10^{-2}$ | $3.6621 \times 10^{-5}$ | 27 | $0.0009888$ | $1.53776 \times 10^{-6}$ | **4.1486** | **2.05** |
| **$128 \times 128$** | $7.8125 \times 10^{-3}$ | $9.1553 \times 10^{-6}$ | 109 | $0.0009979$ | $3.87850 \times 10^{-7}$ | **3.9649** | **1.99** |

#### 2. 3D Convergence ($r = 0.10, \alpha = 1.0$)
| Resolution ($N^3$) | $\Delta x$ | $\Delta t$ | Steps | $t_{\text{final}}$ | $L_2$ Error Norm | Reduction Ratio | Empirical Order ($p$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **$24^3$** | $4.1667 \times 10^{-2}$ | $1.7361 \times 10^{-4}$ | 6 | $0.0010417$ | $1.21020 \times 10^{-5}$ | — | — |
| **$48^3$** | $2.0833 \times 10^{-2}$ | $4.3403 \times 10^{-5}$ | 23 | $0.0009983$ | $2.89885 \times 10^{-6}$ | **4.1748** | **2.06** |
| **$96^3$** | $1.0417 \times 10^{-2}$ | $1.0851 \times 10^{-5}$ | 92 | $0.0009983$ | $7.24440 \times 10^{-7}$ | **4.0015** | **2.00** |

Both schemes exhibit asymptotic $\mathcal{O}(\Delta x^2)$ convergence.

### C. Full Suite Leak Detection Output (`ASAN_OPTIONS=detect_leaks=1`)
Executed all 10 tests across both binaries (`test_config` and `test_serial_solver`):
- `test_config`: 3/3 passed, 0 leaks.
- `test_serial_solver`: 7/7 passed, 0 leaks.
- Total test time under ASan + UBSan: 17.02s.

### D. Native Clang-Tidy Clean Output
- Installed matching GCC/Clang system headers (`libstdc++-16-dev`) resolving the previous header include diagnostic.
- Executed native `clang-tidy` command using compile commands database across all source and test files:
  `clang-tidy -p build/debug src/*.cpp tests/*.cpp`
- **Output**: 0 diagnostics in user code (100% clean).

## 4. Architecture Decision Records (ADRs) Recorded
- `docs/adr/0003-convergence-methodology.md`: Documented full-scheme vs. pure spatial convergence verification methodology and discrete $L_2$ norm definition.

## 5. Next Gate (G2)
- **Gate 2**: MPI 2D/3D Cartesian domain decomposition (`MPI_Cart_create`), non-blocking halo exchanges (`MPI_Isend` / `MPI_Irecv` with `MPI_Waitall`), explicit ghost-to-neighbor verification test, and parallel-vs-serial numerical equivalence across 1 to 8 ranks (including uneven process counts).
