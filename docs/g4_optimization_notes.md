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
- **Plane Size**: A single $XY$-plane of doubles is $514 \times 514 \times 8\text{ bytes} \approx 2.11\text{ MB}$ ($512^2 \times 8\text{ bytes} = 2.097\text{ MB}$).
- **Active 3-Plane Working Set**:
  - Updating a row $(i, j, k)$ requires accessing $(i, j, k-1)$, $(i, j, k)$, and $(i, j, k+1)$.
  - 3 planes require: $3 \times 2.11\text{ MB} \approx 6.33\text{ MB}$.
  - **L2 Cache Eviction**: Because $6.33\text{ MB} \gg 1.25\text{ MB}$ (exceeding L2 by $\approx 5\times$), planes $k-1$ and $k$ are evicted from L2 as the solver iterates sequentially through plane $k$.
  - **L3 Interaction**: The 18 MB shared L3 cache can theoretically hold the 6.33 MB working set. However, write-allocate traffic from updating $u_{\text{new}}$ plus associativity conflicts across the large stride ($514^2 \times 8\text{ bytes} \approx 2.11\text{ MB}$) can evict lines from L3 to DRAM.

### 3. Tiling Traversal & Bounded Prediction
- **Proposed Tiling Shape**:
  - Spatial block of size $B_z \times B_y$ with full unit-stride row $X$ ($B_x = 512$):
    ```cpp
    for (int kk = 1; kk <= nz; kk += Bz)
      for (int jj = 1; jj <= ny; jj += By)
        for (int k = kk; k <= k_end; ++k)
          for (int j = jj; j <= j_end; ++j)
            #pragma omp simd
            for (int i = 1; i <= nx; ++i)
    ```
  - For $B_y = 32$ and $B_z = 32$, a single sub-plane slice is $32 \times 514 \times 8\text{ bytes} \approx 131\text{ KB}$.
  - The 3-slice working set is $3 \times 131\text{ KB} \approx 393\text{ KB}$, which fits inside the $1.25\text{ MB}$ L2 cache.
- **Traffic Reduction Bound & Bounded Prediction**:
  - In unblocked traversal, plane $k$ is loaded for step $k-1$, re-read for step $k$, and re-read for step $k+1$. If evicted from L2, each point incurs multiple L3/DRAM read requests.
  - Tiling retains sub-planes in L2, converting capacity misses into L2 hits.
  - **Theoretical Bound**: Because memory bandwidth to L3/DRAM accounts for approximately $30\%\text{ to }40\%$ of the execution time on modern architectures for arithmetic-light stencils (flop-to-byte ratio $\approx 0.17$), eliminating L2 capacity misses yields a **bounded theoretical gain of $5\%\text{ to }15\%$**.
  - If hardware streaming prefetchers already hide L3 latency during sequential $Z$-plane traversal, the observed gain may be smaller (or neutral).

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
- **Spread**: **$2.398984\text{ s}$** ($9.5\%$ of median, comparable in relative terms to the 2D spread of $0.139\text{ s}$ on $2.754\text{ s}$, which was $5.1\%$).
- The median ($25.18\text{ s}$) represents the canonical figure for all subsequent G4 3D comparisons.
