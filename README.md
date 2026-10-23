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
- C++17 compliant compiler: Tested and validated on GCC (`g++ 15.2.0`, `Ubuntu 15.2.0-16ubuntu1`)
- Standard library headers matching compiler (`libstdc++-16-dev` on Ubuntu 26.04)
- CMake >= 3.20
- MPI: OpenMPI 5.0.10 (`libopenmpi-dev` & `openmpi-bin`)
- OpenMP runtime (`libgomp1`)
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

## Benchmarking Scope, Baselines & Known Limitations

- **Hardware Scope**: Benchmarks on hybrid Intel Alder Lake architectures (e.g., i5-12500H) are strictly scoped to the dedicated **Performance cores (P-cores 0–3)** mapped via `--bind-to core` (logical CPUs 0, 2, 4, 6) to avoid heterogeneous performance throttling. Scaling past 4 cores onto Efficient cores (E-cores) is outside the current milestone scope.
- **Benchmark Measurement Protocol & Baseline**: The benchmark table in `bench/benchmark_g3_summary.csv` is produced by `bench/run_g3_benchmarks.sh`. For each configuration, a separate `mpirun` process executes an unrecorded warmup run, followed by 5 recorded fresh process invocations. In each run, the timer in `src/main.cpp` wraps the complete lifecycle: constructor, Cartesian decomposition, allocation of two $4096 \times 4096$ grids ($2 \times 134.2\text{ MB}$), initial condition computation, 100 FTCS time-stepping iterations, and grid return ($3.266\text{ s}$ median for $4096^2$ 1R1T). In a side-by-side test, fresh-process invocations were slower and more variable in this sample ($2.924\text{ s}$ median, $0.224\text{ s}$ spread) than in-process repeated executions ($2.755\text{ s}$ median, $0.125\text{ s}$ spread); the cause was not isolated. In persistent in-process loops (such as the 10-repetition phase profiler `measure_10reps_phases.cpp`), steady-state compute loops measure $\approx 2.67\text{ s}$ for the compute phase alone and $\approx 2.80\text{ s}$ for the full in-process lifecycle.
- **Serial vs. MPI Comparison**: The serial solver and single-rank MPI solver compile to the same instruction sequence in their inner compute loops (16-byte SSE2), except loop-bound register allocation (`%r11` in serial vs. `%rbx` in OpenMP; 16-byte SSE2), verified by address-stripped diff of the committed raw files (`bench/serial_inner_loop_raw.s` and `bench/mpi_inner_loop_raw.s`). When measured under identical in-process timer boundaries across 10 repetitions (`measure_10reps_phases.cpp`), the median per-run sum is $2.649\text{ s}$ for serial (spread: $0.166\text{ s}$) and $2.670\text{ s}$ for MPI 1R1T (spread: $0.128\text{ s}$). The $0.021\text{ s}$ difference between medians is substantially smaller than the run-to-run spreads of both binaries ($0.166\text{ s}$ and $0.128\text{ s}$), showing no measurable difference at this sample size.
- **Small Grid Multi-Threading ($512^2$ 1R4T)**: On small subdomains ($512 \times 512$, ~0.1 s runtimes), scaling efficiency drops to ~31.7% with 4 threads on 1 rank. Hypothesized causes include OpenMP thread barrier synchronization overhead and cache-line contention relative to the short computation duration; larger subdomains ($4096 \times 4096$) sustain linear and near-linear scaling.
- **CPU Frequency Scaling**: Under virtualized environments (e.g. WSL2), hardware CPU frequency governors (`/sys/devices/system/cpu/cpu*/cpufreq`) are virtualized and real-time per-core clock frequencies are not dynamically sampled during benchmark runs.

