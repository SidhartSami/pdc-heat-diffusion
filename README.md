# Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

[![CI](https://github.com/placeholder/heat-diffusion-parallel/actions/workflows/ci.yml/badge.svg)](https://github.com/placeholder/heat-diffusion-parallel/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

Course Reference: Project #12 (Heat Diffusion / Stencil Computation), Hybrid MPI + OpenMP model.

## Overview
This repository implements a high-performance, distributed-memory (MPI) and shared-memory (OpenMP) numerical solver for the 2D and 3D heat diffusion equation:

$$\frac{\partial u}{\partial t} = \alpha \nabla^2 u$$

using an explicit Forward-Time Central-Space (FTCS) finite-difference scheme.

## Features
- **2D & 3D FTCS Stencil**: 5-point (2D) and 7-point (3D) explicit stencils with configurable boundaries (Dirichlet & Neumann).
- **Hybrid Parallelism**: Domain decomposition with MPI Cartesian communicators (`MPI_Cart_create`) combined with multithreaded OpenMP execution inside each subdomain.
- **Compute-Communication Overlap**: Non-blocking halo exchange (`MPI_Isend`/`MPI_Irecv`) overlapped with interior grid computation.
- **Optimizations**: Cache blocking, SIMD alignment, and NUMA-aware first-touch initialization.
- **Strict Verification & Rigorous Testing**: Analytic convergence tests (sine mode decay), serial-parallel equivalence, energy conservation, and stability limit verification.
- **Reproducible Experimentation**: Config-driven runs, programmatic system metadata capture, automated benchmarks, and visualization scripts.

## Quick Start

### Prerequisites
- C++17 compliant compiler (`g++ >= 11` or `clang++ >= 14`)
- Standard library headers matching compiler (e.g. `libstdc++-16-dev` on Ubuntu 26.04)
- CMake >= 3.20
- MPI (OpenMPI or MPICH, e.g. `libopenmpi-dev` & `openmpi-bin`)
- OpenMP runtime (`libomp-dev`)
- Python 3 with `numpy`, `matplotlib`, `pandas` (for analysis & visualization)

### Build with CMake Presets
```bash
# Optimized Release Build
cmake --preset release
cmake --build --preset release

# Run Tests
ctest --preset release --output-on-failure

# Debug Build with ASan & UBSan
cmake --preset asan-ubsan
cmake --build --preset asan-ubsan
ctest --preset asan-ubsan --output-on-failure
```

### Running the Solver
```bash
# Run 2D simulation on 4 MPI ranks with 2 OpenMP threads each
export OMP_NUM_THREADS=2
mpirun -np 4 ./build/release/heat_solver --config=configs/small_2d.cfg
```

## Directory Structure
- `include/heat/`: Public C++ headers (domain, decomposition, halo exchange, solvers, boundaries, config, timers).
- `src/`: Implementation files and driver `main.cpp`.
- `tests/`: GoogleTest test suites.
- `bench/`: Automated benchmark execution scripts.
- `analysis/`: Python scripts for data processing and figure generation.
- `configs/`: Sample simulation configuration files.
- `docs/`: Architecture Decision Records (`docs/adr/`), proposal, viva preparation guide, and report.
- `results/`: Raw CSV benchmark outputs with complete machine metadata.
