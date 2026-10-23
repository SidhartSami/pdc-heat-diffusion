# Gate 4 Optimization Notes & Hypothesis Ledger

## Task 1: 2D Spatial Cache Blocking ($4096 \times 4096$) — Result: Null (Reverted)
- **Analytical Prediction**: Natural row-major streaming of $4096 \times 4096$ doubles requires 3 active contiguous rows ($3 \times 32\text{ KB} = 96\text{ KB}$), which fits into the dedicated $1.25\text{ MB}$ L2 cache of the P-core. Spatial tiling along $Y$ was predicted to yield $<5\%$ change.
- **Empirical Baseline**: $2.754448\text{ s}$ median ($0.139238\text{ s}$ spread across 10 repetitions, Core 0 pinned).
- **Outcomes**:
  - $B_y = 64$: Median $2.833216\text{ s}$, spread $0.220914\text{ s}$ (no measurable difference from baseline given noise).
  - $B_y = 128$: Median $2.757988\text{ s}$, spread $0.404714\text{ s}$ (full 10-run spread driven by an isolated post-hoc outlier at $3.110\text{ s}$; 9-run subset spread was $0.174\text{ s}$).
  - $B_y = 256$: Median $2.758615\text{ s}$, spread $0.100468\text{ s}$.
- **Resolution**: Neutral result across all parameters. Code cleanly reverted in commit `e490e80` to preserve codebase simplicity, maintaining full test suite invariance (17/17 tests passing).

---

## Task 2: 3D Stencil Optimization ($512 \times 512 \times 512$) — Pre-Measurement Analysis & Bounded Prediction

*(Note: Revised post-baseline to correct exact memory totals and explicitly bound the theoretical cache traffic reduction before any code change is implemented).*

### 1. Memory and Hardware Hierarchy Context
- **Domain Dimensions**: $512 \times 512 \times 512$ interior cells ($134,217,728$ points).
- **Corrected Memory Footprint**:
  - Padded dimensions per buffer: $514 \times 514 \times 514 = 135,796,744$ elements.
  - Per grid buffer: $135,796,744 \times 8\text{ bytes} \approx 1.086\text{ GB}$ ($1.012\text{ GiB}$).
  - Double-buffering total ($u_{\text{old}}$ and $u_{\text{new}}$): **$2.17\text{ GB}$ ($2.02\text{ GiB}$)**.
- **Host System State**:
  - WSL2 available memory: $7044\text{ MB}$ free ($7.04\text{ GB}$), swap is $0\text{ MB}$.
  - Load average prior to baseline: `0.32, 0.07, 0.02`, CPU frequency: `3110.402 MHz`.
  - The working set fits in physical memory with zero swapping.
- **Cache Topology (Intel Core i5-12500H P-core)**:
  - L1D Cache: $48\text{ KB}$ per P-core.
  - L2 Cache: $1.25\text{ MB}$ per P-core.
  - L3 Cache (Shared LLC): $18\text{ MB}$.

### 2. Theoretical Analysis of 3D Stencil Reuse & Traffic Bound
- **Plane Size**: A single $XY$-plane of doubles is $514 \times 514 \times 8\text{ bytes} \approx 2.11\text{ MB}$.
- **Complete Working Set (Read Streams + Destination Write Stream)**:
  - To update row $(i, j, k)$, the stencil reads three input planes of $u_{\text{old}}$ (planes $k-1, k, k+1$) and writes to the destination plane of $u_{\text{new}}$ (plane $k$).
  - Total active 4-plane working set:
    $$3 \text{ read planes } (3 \times 2.11\text{ MB} = 6.33\text{ MB}) + 1 \text{ write plane } (2.11\text{ MB}) = \mathbf{8.44\text{ MB}}$$
  - **L2 Cache Eviction**: $8.44\text{ MB} \gg 1.25\text{ MB}$ (exceeding L2 capacity by $\approx 6.75\times$). During unblocked sequential traversal through plane $k$, earlier cells of planes $k-1, k,$ and $k+1$ are evicted from L2 cache.
  - **L3 Interaction Expectation**: The 18 MB shared L3 cache is expected to comfortably hold the 8.44 MB working set. On this machine, sequential streaming with hardware prefetchers can service cache misses from L3 (latency $\sim 30\text{–}40\text{ cycles}$) much faster than main memory DRAM ($\sim 150\text{–}200\text{ cycles}$). Therefore, L3 retention is expected to dominate, meaning the performance gain from L2 spatial blocking is capped by the L3-to-L2 bandwidth/latency reduction, rather than DRAM memory-bandwidth starvation.

### 3. Tiling Traversal & Bounded Prediction
- **Proposed Tiling Shape**:
  - Spatial block of size $B_z \times B_y$ with full unit-stride row $X$ ($B_x = 512$):
    ```cpp
    for (int kk = 1; kk <= nz; kk += Bz)
      for (int jj = 1; jj <= ny; jj += By)
        for (int k = kk; k <= std::min(kk + Bz - 1, nz); ++k)
          for (int j = jj; j <= std::min(jj + By - 1, ny); ++j)
            for (int i = 1; i <= nx; ++i)
    ```
  - For $B_y = 32$ and $B_z = 32$, a single sub-plane slice is $32 \times 514 \times 8\text{ bytes} \approx 131\text{ KB}$.
  - The complete 4-slice working set (3 read slices + 1 destination write slice) is:
    $$4 \times 131\text{ KB} = \mathbf{524\text{ KB}}$$
  - $524\text{ KB} < 1.25\text{ MB}$, fitting inside the dedicated L2 cache.
- **Traffic Reduction Bound & Bounded Prediction**:
  - **Derivation of the 5–15% Gain Bound**:
    - Modern P-core roofline models for explicit 7-point 3D stencils show arithmetic execution ($8\text{ FLOPs}$ per cell) requires $\sim 2\text{ cycles}$ with 16-byte SSE2 vectors ($1\text{ vector ADD} + 1\text{ vector MUL}$ every 2 doubles).
    - Unblocked streaming incurs L2 misses to L3. L3 hit latency ($\approx 35\text{ cycles}$) is partially hidden by hardware stream prefetchers, but latency stalls and L2 line allocation overhead are estimated to account for roughly $20\%\text{ to }30\%$ of total loop cycles.
    - By reducing L2 capacity misses on planes $k-1$ and $k$ from L3 down to L2 hit latencies ($\approx 12\text{ cycles}$), we save roughly half of those L2 miss stall cycles ($0.5 \times 20\% \approx 10\%$).
    - This calculation bounds the theoretical expectation to **$5\%\text{ to }15\%$**.
    - If hardware prefetchers already hide L3 access completely during sequential row streaming, the observed speedup may be smaller or neutral within run-to-run noise.

---

### 4. Empirical Pre-Optimization 3D Baseline (Commit Record)

Measured on `configs/large_3d.cfg` (50 steps, Dirichlet sine mode, 1R1T, Core 0 pinned) under 1 unrecorded warmup followed by 10 recorded repetitions:

```text
[3D Rep  1/10] 25.756374 s
[3D Rep  2/10] 24.808514 s
[3D Rep  3/10] 24.458555 s
[3D Rep  4/10] 25.370012 s
[3D Rep  5/10] 24.989752 s
[3D Rep  6/10] 24.563845 s
[3D Rep  7/10] 25.929785 s
[3D Rep  8/10] 26.857539 s
[3D Rep  9/10] 26.357677 s
[3D Rep 10/10] 24.801733 s
```

- **3D Pre-Optimization Baseline Median**: **$25.179882\text{ s}$**
- **Min**: $24.458555\text{ s}$
- **Max**: $26.857539\text{ s}$
- **Spread**: **$2.398984\text{ s}$** ($9.5\%$ of median, comparable in relative terms to the 2D baseline spread of $5.1\%$).
- Reps 7, 8, and 9 were slower ($25.9\text{ s}$–$26.9\text{ s}$); the cause is unexplained (possibly background OS scheduling or thermal variance over the ~4.5 minute continuous run). The median ($25.18\text{ s}$) represents the baseline reference.
