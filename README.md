# Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

<p align="center">
  <img src="analysis/figures/demo_diffusion.gif" alt="Parallel 2D Heat Diffusion Simulation" width="70%" />
</p>

<p align="center">
  <a href="https://github.com/SidhartSami/pdc-heat-diffusion/actions/workflows/ci.yml"><img src="https://github.com/SidhartSami/pdc-heat-diffusion/actions/workflows/ci.yml/badge.svg" alt="CI Pipeline" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT" /></a>
  <a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=cplusplus" alt="C++17 Standard" /></a>
  <img src="https://img.shields.io/badge/MPI-OpenMPI%205.0-red.svg" alt="OpenMPI" />
  <img src="https://img.shields.io/badge/OpenMP-4.5%20%2F%20NUMA-orange.svg" alt="OpenMP" />
  <img src="https://img.shields.io/badge/Architecture-x86__64%20%2F%20AVX2-lightgrey.svg" alt="x86_64" />
</p>

---

## Table of Contents
- [Authors & Course Reference](#authors--course-reference)
- [Executive Overview](#executive-overview)
- [Mathematical Formulation & Stability](#mathematical-formulation--stability)
- [System Architecture & Parallel Design](#system-architecture--parallel-design)
- [Performance & Scaling Results](#performance--scaling-results)
- [Quick Start & Build Instructions](#quick-start--build-instructions)
- [CLI Parameter Configuration](#cli-parameter-configuration)
- [Verification & Numerical Invariants](#verification--numerical-invariants)
- [Repository Structure](#repository-structure)
- [Academic Citation](#academic-citation)

---

## Authors & Course Reference

- **Sidhart Sami** (23I-2527)
- **Abdul Rafy** (23P-0560) — [@Abdul-Rafy2005](https://github.com/Abdul-Rafy2005)

* **Institution**: FAST-NUCES
* **Course**: Parallel and Distributed Computing (PDC), Section BCS-7A
* **Project Reference**: Project #12 (Heat Diffusion / Stencil Computation), Hybrid Distributed-Shared Memory Model
* **Documentation**: Final Report in [`docs/report/report.md`](docs/report/report.md), Architecture Guide in [`docs/hpc_architecture_guide.md`](docs/hpc_architecture_guide.md), Viva Defense in [`docs/analytical_roofline_and_defense.md`](docs/analytical_roofline_and_defense.md).

---

## Executive Overview

This repository implements an enterprise-grade, massively parallel numerical solver for transient thermal diffusion governed by the parabolic partial differential equation:

$$\frac{\partial u}{\partial t} = \alpha \nabla^2 u = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} + \left[\frac{\partial^2 u}{\partial z^2}\right] \right)$$

Discretized using an explicit **Forward-Time Central-Space (FTCS)** finite-difference scheme with a 5-point stencil in 2D and 7-point stencil in 3D. 

The solver combines **distributed-memory domain decomposition via MPI Cartesian topologies** with **shared-memory multi-threaded loop execution via OpenMP**, supporting non-blocking compute-communication overlap, NUMA-aware first-touch memory allocation, spatial cache blocking, parallel VTK output, and bit-exact checkpoint/restart.

---

## Mathematical Formulation & Stability

### Explicit FTCS Stencils
- **2D 5-Point Update**:
  $$u^{n+1}_{i,j} = u^n_{i,j} + \Delta t \left( \frac{\alpha}{\Delta x^2} (u^n_{i+1,j} - 2u^n_{i,j} + u^n_{i-1,j}) + \frac{\alpha}{\Delta y^2} (u^n_{i,j+1} - 2u^n_{i,j} + u^n_{i,j-1}) \right)$$
- **3D 7-Point Update**:
  $$u^{n+1}_{i,j,k} = u^n_{i,j,k} + \alpha \Delta t \sum_{d \in \{x,y,z\}} \frac{u^n_{d+1} - 2u^n_d + u^n_{d-1}}{\Delta d^2}$$

### Numerical Stability Limits (Von Neumann / CFL Limit)
Numerical stability requires bounded amplification factors ($|\xi| \le 1$), enforcing:
$$\Delta t \le \frac{1}{2 \alpha \sum_{d=1}^{\text{dim}} \frac{1}{\Delta x_d^2}}$$
- In 2D with uniform spacing: $r = \frac{\alpha \Delta t}{\Delta x^2} \le 0.25$
- In 3D with uniform spacing: $r = \frac{\alpha \Delta t}{\Delta x^2} \le \frac{1}{6} \approx 0.1667$
- The solver automatically guards against instability; `--dt=auto` selects $0.9 \times \Delta t_{\text{max}}$.

---

## System Architecture & Parallel Design

```
+-----------------------------------------------------------------------------------+
|                           MPI Cartesian Topology Grid                             |
|                                                                                   |
|   +----------------------------+             +----------------------------+       |
|   |   MPI Rank 0 (Subdomain)   |  Halo Slices|   MPI Rank 1 (Subdomain)   |       |
|   |  +----------------------+  |<----------->|  +----------------------+  |       |
|   |  | Boundary Shell Cells |  | MPI_Isend   |  | Boundary Shell Cells |  |       |
|   |  |  +----------------+  |  | MPI_Irecv   |  |  +----------------+  |  |       |
|   |  |  | Interior Grid  |  |  |             |  |  | Interior Grid  |  |  |       |
|   |  |  | (OpenMP Loop)  |  |  |             |  |  | (OpenMP Loop)  |  |  |       |
|   |  |  +----------------+  |  |             |  |  +----------------+  |  |       |
|   |  +----------------------+  |             |  +----------------------+  |       |
|   +----------------------------+             +----------------------------+       |
|                 ▲                                           ▲                     |
|                 │ Non-blocking Ghost Exchange               │ Non-blocking Halo   |
|                 ▼                                           ▼                     |
|   +----------------------------+             +----------------------------+       |
|   |   MPI Rank 2 (Subdomain)   |<----------->|   MPI Rank 3 (Subdomain)   |       |
|   +----------------------------+             +----------------------------+       |
+-----------------------------------------------------------------------------------+
```

### Key Engineering Highlights
1. **Contiguous Buffer Packing (ADR 0004)**: Eliminates strided network access; pre-allocated contiguous buffers feed non-blocking `MPI_Isend` and `MPI_Irecv` requests.
2. **Compute-Communication Overlap**: Overlaps halo exchange latency with interior domain computation before synchronizing via `MPI_Waitall` to update the boundary shell.
3. **NUMA-Aware Memory First-Touch**: Grid memory is parallel-initialized with `#pragma omp parallel for schedule(static)` matching the stencil compute partition, binding physical DRAM pages to the local executing core.
4. **3D Spatial Cache Blocking ($B_z \times B_y$)**: Constrains the active 4-plane working set within the dedicated $1.25\text{ MB}$ L2 cache of the host processor, reducing DRAM latency stalls by $6.3\%$.
5. **Parallel Scalable I/O**: Emits per-rank `.vti` XML ImageData pieces and master `.pvti` / `.pvd` time-series headers. ParaView-ready with zero rank-0 gathering bottlenecks.
6. **Bit-Exact Fault-Tolerant Checkpoint/Restart**: Atomic temporary file write and POSIX rename (`_r<rank>.bin`) with collective step verification across all ranks.

---

## Performance & Scaling Results

Benchmarked strictly on physical **Performance cores (P-cores 0, 2, 4, 6)** of an Intel Core i5-12500H under WSL2 using dedicated core pinning (`taskset` + `mpirun --bind-to core`).

### 1. Strong Scaling Performance
- **2D Grid ($4096 \times 4096$, 100 steps)**:
  - **1 Core (MPI P=1)**: 3.126 s (1.00x) — 536.7 MegaUpdates/s
  - **2 Cores (MPI P=2)**: 1.824 s (**1.71x**, 85.7% efficiency) — 920.1 MegaUpdates/s
  - **4 Cores (MPI P=4)**: **1.117 s (2.80x, 70.0% efficiency)** — **1502.6 MegaUpdates/s**
  - **4 Cores (OpenMP T=4)**: 1.720 s (1.82x, 45.4% efficiency) — 975.3 MegaUpdates/s
  - **4 Cores (Hybrid 2R x 2T)**: 1.376 s (2.27x, 56.8% efficiency) — 1219.1 MegaUpdates/s

- **3D Grid ($512^3$, 50 steps, $B_z=B_y=32$)**:
  - **1 Core (MPI P=1)**: 22.34 s (1.00x) — 300.4 MegaUpdates/s
  - **2 Cores (MPI P=2)**: 11.07 s (**2.02x**, 100.9% efficiency) — 606.5 MegaUpdates/s
  - **4 Cores (MPI P=4)**: **6.93 s (3.22x, 80.6% efficiency)** — **968.4 MegaUpdates/s**
  - **4 Cores (OpenMP T=4)**: 14.92 s (1.50x, 37.4% efficiency) — 449.8 MegaUpdates/s
  - **4 Cores (Hybrid 2R x 2T)**: 9.21 s (2.42x, 60.6% efficiency) — 728.5 MegaUpdates/s

### 2. Proposal Problem Size ($8192 \times 8192$, 5000 Steps, 1.07 GB Memory Footprint)
| Configuration | Cores | Wall Time | Speedup | Parallel Efficiency |
|---|---|---|---|---|
| Sequential Reference (MPI P=1) | 1 Core | 452.6 s (7.5 min) | 1.00x | 100.0% |
| Distributed MPI (P=4) | 4 Cores | **184.8 s (3.1 min)** | **2.45x** | **61.2%** |
| Hybrid Parallel (P=2, T=2) | 4 Cores | 190.1 s (3.2 min) | 2.38x | 59.6% |

### 3. Roofline & Bottleneck Insights
- **Arithmetic Intensity**: $\approx 6 \text{ FLOPs} / 48 \text{ bytes} \approx 0.125 \text{ FLOPs/byte}$.
- Because the stencil is strictly **memory-bandwidth bound**, multi-core scaling efficiency is governed by memory bus saturation rather than raw arithmetic ALU limits.
- Pure MPI outperforms pure OpenMP due to private compact subdomains and the absence of per-step thread barrier synchronization.

---

## Quick Start & Build Instructions

### Prerequisites
- C++17 compliant compiler (`g++ >= 11` or `clang++ >= 14`)
- CMake `>= 3.20`
- OpenMPI `>= 4.1` (`libopenmpi-dev`, `openmpi-bin`)
- OpenMP runtime (`libgomp1`)
- Python 3 with `numpy`, `matplotlib`, `pandas` (for visualization)

### Build with CMake Presets or Makefile

```bash
# Clone the repository
git clone https://github.com/SidhartSami/pdc-heat-diffusion.git
cd pdc-heat-diffusion

# Option A: Build and test using Makefile
make build
make test
make preflight

# Option B: Build using CMake Presets directly
cmake --preset release
cmake --build --preset release -j
ctest --preset release --output-on-failure
```

### Complete End-to-End Demo
Executes serial-parallel equivalence, scaling verification, snapshot visualization, and checkpoint/restart verification:
```bash
./demo.sh
```

---

## CLI Parameter Configuration

The solver accepts key=value flags and configuration files (`--config=configs/...`):

```bash
# Execute 4 MPI ranks with 2 OpenMP threads each
export OMP_NUM_THREADS=2
mpirun -np 4 ./build/release/heat_solver --config=configs/small_2d.cfg

# CLI overrides example:
mpirun -np 4 ./build/release/heat_solver \
  --dim=2 --nx=2048 --ny=2048 --steps=500 --dt=auto \
  --ic=gaussian --boundary=dirichlet --overlap=true \
  --snapshot-freq=50 --output=output/sim_data \
  --checkpoint-freq=250 --checkpoint-path=output/ckpt
```

| Parameter | Type | Default | Description |
|---|---|---|---|
| `--dim` | int | `2` | Spatial dimension (`2` for 2D, `3` for 3D) |
| `--nx`, `--ny`, `--nz` | int | `64` | Physical interior grid resolution per dimension |
| `--dt` | float/str | `0.0001` | Time increment (`--dt=auto` enforces $0.9 \times \text{CFL}$) |
| `--steps` | int | `100` | Total time-stepping iterations |
| `--alpha` | float | `1.0` | Thermal diffusivity coefficient |
| `--ic` | str | `sine` | Initial condition (`sine`, `gaussian`, `step`) |
| `--boundary` | str | `dirichlet`| Boundary condition (`dirichlet`, `neumann`) |
| `--overlap` | bool | `false` | Enable non-blocking compute-communication overlap |
| `--bz`, `--by` | int | `0` | 3D spatial cache block sizes (0 = full dimension) |
| `--snapshot-freq` | int | `0` | Frequency of ParaView VTK snapshot exports (0 = off) |
| `--checkpoint-freq`| int | `0` | Checkpoint write frequency in steps (0 = off) |
| `--restart-from` | str | `""` | Path prefix of checkpoint binary to resume from |

---

## Verification & Numerical Invariants

- **Bit-Identical Equivalence**: MPI distributed grid output matches serial reference output to exact bit precision (**`Max Diff = 0.0`**) across $P \in \{1, 2, 3, 4, 6, 8\}$ ranks on uneven grids ($67 \times 53$ and $37 \times 29 \times 31$).
- **Convergence Verification**: Second-order spatial convergence $\mathcal{O}(\Delta x^2)$ confirmed against analytic sine mode decay.
- **Energy Conservation**: Total thermal energy is strictly conserved under zero-flux Neumann boundaries with relative drift $\le 10^{-12}$.
- **Memory Safety**: 100% clean under GCC AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan), and LeakSanitizer (LSan).

```bash
# Run automated numerical reproducibility compliance audit
python scripts/verify_reproducibility.py
```

---

## Repository Structure

```
pdc-heat-diffusion/
├── include/heat/            # Public C++17 modular headers
│   ├── grid.hpp             # Contiguous 1D/2D/3D grids with ghost layers
│   ├── decomposition.hpp    # Cartesian process grid & remainder distribution
│   ├── halo.hpp             # Contiguous buffer packing & non-blocking transfers
│   ├── solver.hpp           # Serial reference FTCS solver
│   ├── mpi_solver.hpp       # Distributed hybrid MPI+OpenMP solver
│   ├── boundary.hpp         # Dirichlet/Neumann conditions & analytic solutions
│   ├── config.hpp           # INI parser and CFL stability validator
│   ├── io.hpp               # Parallel VTI/PVTI/PVD writer & binary checkpointing
│   └── timer.hpp            # High-resolution MPI collective timing
├── src/                     # C++ implementations and main driver
├── tests/                   # 23 GoogleTest suites (Serial, MPI, Sanitizers)
├── scripts/                 # Diagnostics & verification harnesses
│   ├── preflight_check.py   # HPC topology & thread-affinity validator
│   └── verify_reproducibility.py # Bit-identical & invariant compliance checker
├── configs/                 # Standardized simulation configuration files
├── bench/                   # Benchmark runners, raw telemetry, assembly diffs
├── analysis/                # Telemetry analysis and visualization
│   ├── visualize.py         # Snapshot renderer (ParaView VTI -> PNG/GIF)
│   ├── plot_roofline.py     # Empirical Roofline model generator
│   └── figures/             # Scaling plots and diffusion animations
├── docs/                    # Technical documentation
│   ├── report/report.md     # Comprehensive academic final report
│   ├── hpc_architecture_guide.md # Topology, striding & state machines
│   ├── analytical_roofline_and_defense.md # Stability proofs & viva prep
│   └── adr/                 # Architecture Decision Records (0001-0005)
├── Makefile                 # Top-level build orchestration workflow
├── CITATION.cff             # Academic Citation File Format
└── CMakeLists.txt           # Modern CMake build configuration
```

---

## Academic Citation

If you reference this implementation, benchmark methodology, or architectural analysis, please cite:

```bibtex
@software{sami_rafy_2026_heatdiffusion,
  author       = {Sidhart Sami and Abdul Rafy},
  title        = {Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)},
  year         = {2026},
  publisher    = {GitHub},
  url          = {https://github.com/SidhartSami/pdc-heat-diffusion},
  version      = {1.0.0}
}
```

---

<p align="center">
  <b>Developed for Parallel and Distributed Computing (PDC) — FAST-NUCES</b>
</p>
