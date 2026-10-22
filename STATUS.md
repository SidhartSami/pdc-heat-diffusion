# Gate 0 (G0) Status Report: Repository Scaffold & Build Infrastructure

## 1. Gate Status: COMPLETE (Awaiting User Review / Approval)

## 2. What Was Done
- **Repository Scaffold & Layout**:
  - `CMakeLists.txt` configured with C++17, OpenMP, MPI, and GoogleTest via `FetchContent`.
  - `CMakePresets.json` configured with three standard presets: `debug`, `release`, and `asan-ubsan`.
  - Formatter & Linter rules: `.clang-format` (LLVM-based 4-space indent) and `.clang-tidy`.
  - Pre-commit hook definitions: `.pre-commit-config.yaml`.
  - Git configuration: `.gitignore` (ignoring builds, deps, outputs, logs, virtual environments).
  - Open Source License: `LICENSE` (MIT).
  - Contributor guide: `CONTRIBUTING.md` detailing branch strategy, PRs, and conventional commit guidelines.
  - Project Proposal: `docs/proposal.md` with problem formulation and target grids ($4096^2$ and $512^3$).
  - Design Document: `docs/design.md` covering memory layouts, Cartesian decomposition, and overlap strategy.
  - Viva Voce Preparation Guide: `docs/viva-prep.md` addressing fundamental oral exam questions.
  - CI Pipeline: `.github/workflows/ci.yml` matrix building `debug`, `release`, `asan-ubsan`, testing via CTest and MPI smoke tests, plus `clang-format` checking.
  - Core configuration & timing primitives:
    - `include/heat/config.hpp` & `src/config.cpp` (custom key=value parser, FTCS stability calculator).
    - `include/heat/timer.hpp` & `src/timer.cpp` (MPI_Wtime wrapper with min/max/avg reduction).
    - `src/main.cpp` (MPI funneled initialization check, config parsing, stability validation guard).
    - `tests/test_config.cpp` (GoogleTest suite testing stability limits and file parsing).

## 3. What Was Verified & How
- **CMake Presets Build**:
  - Debug build configured and compiled with `-Wall -Wextra -Wpedantic -Werror`.
  - Sanitizer build (`asan-ubsan`) configured and compiled cleanly with `-fsanitize=address,undefined`.
- **GoogleTest Suite**:
  - 100% tests passed under both Debug and ASan/UBSan runs (`3/3 passed`).
- **Formatting**:
  - Verified with `find include src tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror` (clean, zero violations).
- **MPI & Stability Enforcement**:
  - `mpirun -np 2 --oversubscribe ./build/debug/heat_solver --config=configs/small_2d.cfg` successfully ran and validated stable parameters.
  - Intentionally unstable config (`configs/unstable_2d.cfg`) was tested and confirmed rejected with exit code 2 and explicit error message:
    `ERROR: Explicit FTCS stability criterion violated! dt = 0.001 > max_stable_dt = 6.10352e-05`.

## 4. Architecture Decision Records (ADRs)
- `docs/adr/0001-use-cpp17-standard.md`: Justified selection of C++17 standard for universal compiler and MPI compatibility.
- `docs/adr/0002-config-parser-choice.md`: Justified dependency-free key=value config parser to minimize build overhead.

## 5. What Is Open / Next Gate (G1)
- **Gate 1**: Implementation of the serial 2D and 3D FTCS solver with double buffering (`u_old`, `u_new`), analytical verification against decaying sine modes, empirical convergence rate tests ($O(\Delta x^2)$ in space, $O(\Delta t)$ in time), and Dirichlet/Neumann boundaries.
