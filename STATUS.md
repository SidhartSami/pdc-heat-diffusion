# Gate 0 (G0) Status Report: Repository Scaffold & Build Infrastructure

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Repository Scaffold & Layout**:
  - `CMakeLists.txt` configured with C++17, OpenMP, MPI, and GoogleTest via `FetchContent`.
  - `CMakePresets.json` configured with three standard presets: `debug`, `release`, and `asan-ubsan`.
  - Formatter & Linter rules: `.clang-format` (LLVM-based 4-space indent) and `.clang-tidy`.
  - Pre-commit hook definitions: `.pre-commit-config.yaml`.
  - Git configuration: `.gitignore` and `.gitkeep` files in `bench/`, `analysis/`, `results/`, and `output/`.
  - Documentation stubs: `docs/report/report.md` and `docs/references.md`.
  - Open Source License: `LICENSE` (MIT).
  - Contributor guide: `CONTRIBUTING.md` detailing branch strategy, PRs, and conventional commit guidelines.
  - Project Proposal: `docs/proposal.md` with problem formulation and target grids ($4096^2$ and $512^3$).
  - Design Document: `docs/design.md` covering memory layouts, Cartesian decomposition, and overlap strategy.
  - Viva Voce Preparation Guide: `docs/viva-prep.md` addressing fundamental oral exam questions.
  - CI Pipeline: `.github/workflows/ci.yml` matrix building `debug`, `release`, `asan-ubsan`, testing via CTest and MPI smoke tests, plus `clang-format` checking.
  - Six standard configuration files:
    - `configs/small_2d.cfg` ($64 \times 64$)
    - `configs/medium_2d.cfg` ($512 \times 512$)
    - `configs/large_2d.cfg` ($4096 \times 4096$)
    - `configs/small_3d.cfg` ($64 \times 64 \times 64$)
    - `configs/medium_3d.cfg` ($128 \times 128 \times 128$)
    - `configs/large_3d.cfg` ($512 \times 512 \times 512$)
    - `configs/unstable_2d.cfg` (intentionally unstable for testing rejection)
  - Core configuration & timing primitives:
    - `include/heat/config.hpp` & `src/config.cpp` (custom key=value parser, FTCS stability calculator).
    - `include/heat/timer.hpp` & `src/timer.cpp` (MPI_Wtime wrapper using the standard MPI C API with min/max/avg reduction).
    - `src/main.cpp` (MPI funneled initialization check, config parsing, stability validation guard).
    - `tests/test_config.cpp` (GoogleTest suite testing stability limits and file parsing).

## 3. What Was Verified & How
- **MPI & OpenMP Interface Standard**:
  - Code uses the standard MPI C API (`#include <mpi.h>`) throughout. Deprecated/removed MPI C++ bindings are not used.
  - OpenMP version reported by `_OPENMP` macro is `201511` (OpenMP 4.5).
- **CMake Presets Build & CTest Verification**:
  - `debug` preset: Built with `-Wall -Wextra -Wpedantic -Werror`, 100% tests passed (`3/3 passed`).
  - `release` preset: Built with `-O3 -DNDEBUG`, 100% tests passed (`3/3 passed`).
  - `asan-ubsan` preset: Built with `-fsanitize=address,undefined`, 100% tests passed (`3/3 passed`).
- **Formatting, Linters, and Pre-commit**:
  - `clang-format --dry-run --Werror`: 0 violations across all `.hpp` and `.cpp` files.
  - `clang-tidy`: Checked with `.clang-tidy` suppressing external OpenMPI macro void-cast warnings.
  - `pre-commit run --all-files`: 100% passed (trailing-whitespace, end-of-file-fixer, check-yaml, check-added-large-files, clang-format).
- **Stability Formula & Numerical Verification**:
  - Physical domain convention: $x \in [0, L]$ with $L = 1.0$. Cell spacing is $\Delta x = L / N = 1 / N$.
  - Thermal diffusivity $\alpha$ is dynamic and read directly from config (`cfg.alpha`).
  - Stability formula is dimension-aware: $\Delta t \le \frac{1}{2 \alpha \sum_{i=1}^d \frac{1}{\Delta x_i^2}}$.
  - Tested hand-computed 3D stability threshold in `StabilityLimitCalculation3D`: for $100^3$ with $\alpha=1.0$, $\Delta t_{\max} = \frac{1}{60000} \approx 1.66667 \times 10^{-5}$, exactly matched by `cfg.maxStableDt()`.
- **MPI Smoke & Stability Enforcement**:
  - Intentionally unstable config (`configs/unstable_2d.cfg`) exits immediately with non-zero exit code (exit code 2) and logs an explicit stability rejection error.
  - Benchmarks will strictly forbid `--oversubscribe` and monitor physical core counts.

## 4. Architecture Decision Records (ADRs)
- `docs/adr/0001-use-cpp17-standard.md`: Justified selection of C++17 standard and pure MPI C API.
- `docs/adr/0002-config-parser-choice.md`: Justified dependency-free key=value config parser.

## 5. What Is Open / Next Gate (G1)
- **Gate 1**: Implementation of the serial 2D and 3D FTCS solver with double buffering (`u_old`, `u_new`), analytical verification against decaying sine modes, empirical convergence rate tests ($O(\Delta x^2)$ in space, $O(\Delta t)$ in time), and Dirichlet/Neumann boundaries.
