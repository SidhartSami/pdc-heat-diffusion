# Gate 5 Scaling & Evaluation Plan

## 1. Overview and Objectives

Gate 5 (G5) evaluates the parallel scaling performance of the heat diffusion solver across multi-core and multi-process configurations.
The objectives are:
1. **Strong Scaling (Fixed Problem Size)**: Measure wall-clock runtime, speedup $S(p)$, and parallel efficiency $E(p)$ as processor count increases for fixed global grid dimensions.
   - 2D domain: $4096 \times 4096$, 100 steps.
   - 3D domain: $512 \times 512 \times 512$, 50 steps.
2. **Weak Scaling (Scaled Problem Size)**: Measure wall-clock runtime and weak scaling efficiency $E_w(p)$ as domain size grows proportionally with processor count, maintaining constant local grid work per core:
   - 2D per-core domain: $2048 \times 2048$ (~4.19M points/core), 100 steps.
   - 3D per-core domain: $256 \times 256 \times 256$ (~16.78M points/core), 30 steps.
3. **Communication-Computation Overlap Evaluation**: Measure paired delta and execution efficiency with non-blocking ghost exchange (`overlap = true`) versus blocking exchange (`overlap = false`) across decomposition boundaries.
4. **Hybrid MPI + OpenMP Exploration**: Compare pure MPI ($P$ ranks, 1 thread) vs pure OpenMP (1 rank, $T$ threads) vs hybrid ($P$ ranks $\times$ $T$ threads) on equivalent physical core allocations.

---

## 2. Hardware Topology and Binding Constraints

- **Processor**: Intel Core i5-12500H (Alder Lake hybrid architecture: 4 Performance cores / 8 threads + 8 Efficient cores).
- **Core Partitioning Policy**:
  - All scaling benchmarks evaluate core counts $N \in \{1, 2, 4\}$.
  - To prevent heterogeneous throttling and core-scheduling variance between P-cores and E-cores, all scaling runs are strictly bound to physical P-cores without SMT sharing:
    - 1 Core: Physical Core 0 (`taskset -c 0`)
    - 2 Cores: Physical Cores 0, 2 (`taskset -c 0,2`)
    - 4 Cores: Physical Cores 0, 2, 4, 6 (`taskset -c 0,2,4,6`)
  - OpenMP threads are bound using `OMP_PLACES=cores OMP_PROC_BIND=close`.
  - MPI ranks are bound using `mpirun --bind-to core`.
  - Frequency / Load telemetry (`/proc/loadavg` and `/proc/cpuinfo` MHz) is logged before and after every execution.

---

## 3. Pre-Committed Decision Rules & Benchmark Protocol

### Protocol Controls
1. **Warmup**: Each configuration executes 1 unmeasured warmup iteration immediately prior to recorded runs to prime caches, page tables, and MPI communication buffers.
2. **Repetition Count**: $N = 7$ recorded runs per configuration point.
3. **Reported Statistics**:
   - Primary metric: Median wall-clock solver step time $T_{\text{median}}$ (seconds).
   - Dispersion metric: Full range spread $(T_{\text{max}} - T_{\text{min}})$ and interquartile range (IQR).
   - Speedup: $S(p) = \frac{T_{\text{median}}(1)}{T_{\text{median}}(p)}$.
   - Parallel Efficiency: $E(p) = \frac{S(p)}{p} \times 100\%$.
4. **Interleaved Overlap Comparison**:
   - For overlap evaluation (`overlap=true` vs `overlap=false`), runs are strictly interleaved in 10 alternating pairs ($N=20$ runs total) with balanced lead order (5 pairs overlap-first, 5 pairs non-overlap-first) to prevent thermal drift bias.
   - Improvement is declared if and only if:
     - Median paired difference $\Delta = T_{\text{overlap}} - T_{\text{non-overlap}} < 0$, AND
     - 95% bootstrap confidence interval of $\Delta$ excludes zero.

---

## 4. Experiment Matrix

### Suite A: 2D Strong Scaling ($4096 \times 4096$, 100 steps)
- Matrix:
  - Pure MPI: $P \in \{1, 2, 4\}$, $T = 1$
  - Pure OpenMP: $P = 1$, $T \in \{1, 2, 4\}$
  - Hybrid MPI+OpenMP: $P = 2, T = 2$ (total 4 cores)

### Suite B: 3D Strong Scaling ($512 \times 512 \times 512$, 50 steps, $B_z=32, B_y=32$)
- Matrix:
  - Pure MPI: $P \in \{1, 2, 4\}$, $T = 1$
  - Pure OpenMP: $P = 1$, $T \in \{1, 2, 4\}$
  - Hybrid MPI+OpenMP: $P = 2, T = 2$ (total 4 cores)

### Suite C: 2D Weak Scaling (Constant Local Work ~4.19M points/core, 100 steps)
- $P = 1$: $2048 \times 2048$ ($4.19\times 10^6$ points)
- $P = 2$: $2048 \times 4096$ ($8.39\times 10^6$ points, 1D grid $1 \times 2$)
- $P = 4$: $4096 \times 4096$ ($16.78\times 10^6$ points, 2D grid $2 \times 2$)

### Suite D: 3D Weak Scaling (Constant Local Work ~16.78M points/core, 30 steps)
- $P = 1$: $256 \times 256 \times 256$ ($16.78\times 10^6$ points)
- $P = 2$: $256 \times 256 \times 512$ ($33.55\times 10^6$ points, 1D grid $1 \times 1 \times 2$)
- $P = 4$: $256 \times 512 \times 512$ ($67.11\times 10^6$ points, 2D grid $1 \times 2 \times 2$)

### Suite E: Non-Blocking Overlap Evaluation ($P=4$ MPI, P-cores 0, 2, 4, 6)
- 2D: $4096 \times 4096$, 100 steps (10 interleaved pairs overlap vs non-overlap)
- 3D: $512 \times 512 \times 512$, 50 steps (10 interleaved pairs overlap vs non-overlap)
