# Contributing Guidelines

Welcome to the **Parallel Heat Diffusion Simulation** project repository. This document outlines the development workflow, branch strategy, formatting requirements, and review guidelines for all team members.

## 1. Branch Strategy and Push Access Control

- `main`: Production-ready, passing all tests and CI checks. Never commit directly to `main`.
- `gate/g<N>-<name>`: Milestone branches corresponding to project gates (e.g., `gate/g0-scaffold`, `gate/g1-serial-solver`, `gate/g2-mpi-decomposition`, `gate/g3-openmp-hybrid`).
- `feature/<name>` or `fix/<name>`: Specific subtasks or bug fixes, merged into the current gate branch via PR.
- **Push Permissions & Integrity**: Only the repository owner (`SidhartSami`) and authorized collaborator (`Abdul-Rafy2005`) hold write access to the repository. Commits pushed to `gate/*` branches and `main` represent evaluated and frozen milestone checkpoints; no unreviewed force-pushes or rebases are permitted once a gate is under review or approved.

## 2. Commit Message Convention

We follow [Conventional Commits](https://www.conventionalcommits.org/):
- `feat:` A new feature (e.g. `feat(stencil): add 3D 7-point FTCS kernel`)
- `fix:` A bug fix (e.g. `fix(halo): handle zero-size ghost buffers on periodic boundary`)
- `perf:` Performance optimization (e.g. `perf(simd): add loop tiling and restrict annotations`)
- `test:` Adding or updating tests (e.g. `test(analytic): add 2D sine wave decay test`)
- `docs:` Documentation only changes (e.g. `docs(adr): add ADR-0001 on C++17 selection`)
- `ci:` Changes to CI configuration files and scripts
- `chore:` Maintenance, build system, or dependency updates

## 3. Code Style and Formatting

- **C++ Formatting**: Enforced via `clang-format` based on Google/LLVM style (see `.clang-format`).
- **Static Analysis**: Checked with `clang-tidy` (see `.clang-tidy`).
- **Pre-commit**: We provide a `.pre-commit-config.yaml` to ensure code is clean before pushing.
- **Doxygen**: All public headers in `include/heat/` must have Doxygen comments (`/** ... */`) documenting classes, functions, parameters, and return types.

## 4. Pull Requests and Gates

1. Every pull request must pass CMake configuration, compilation with `-Wall -Wextra -Wpedantic -Werror`, tests across multiple MPI rank counts, and sanitizer checks (ASan + UBSan).
2. Architectural decisions must be documented in `docs/adr/` using the MADR template before merging significant structural changes.
3. Keep PRs focused on a single concern.
