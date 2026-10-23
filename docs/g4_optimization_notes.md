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

## Task 2: 3D Stencil Optimization ($512 \times 512 \times 512$) — Pre-Measurement Analysis & Prediction

### 1. Memory and Hardware Hierarchy Context
- **Domain Dimensions**: $512 \times 512 \times 512$ cells ($134,217,728$ grid points).
- **Memory Footprint**:
  - $u_{\text{old}}$: $514 \times 514 \times 514 \times 8\text{ bytes} \approx 1.087\text{ GB}$ (with ghost cells).
  - $u_{\text{new}}$: $514 \times 514 \times 514 \times 8\text{ bytes} \approx 1.087\text{ GB}$.
  - Double-buffering total working set: $\approx 2.17\text{ GB}$.
- **Host Availability Verified**:
  - Available memory in WSL2: $7044\text{ MB}$ free ($7.04\text{ GB}$), swap is $0\text{ MB}$.
  - The working set ($\approx 2.17\text{ GB}$) fits entirely in physical RAM with zero OS swapping risk.
- **Cache Topology (Intel Core i5-12500H P-core)**:
  - L1D Cache: $48\text{ KB}$ per P-core.
  - L2 Cache: $1.25\text{ MB}$ per P-core.
  - L3 Cache (Shared LLC): $18\text{ MB}$.

### 2. Theoretical Analysis of 3D Stencil Reuse
- **7-Point Stencil Access Pattern**:
  To update point $(i, j, k)$, the stencil reads:
  $$(i \pm 1, j, k) \quad \text{[same row]}, \quad (i, j \pm 1, k) \quad \text{[same plane]}, \quad (i, j, k \pm 1) \quad \text{[adjacent planes]}$$
- **The $Z$-Plane Cache Capacity Bottleneck**:
  - A single $XY$-plane consists of $512 \times 512 \times 8\text{ bytes} = 2.097\text{ MB}$.
  - The 3-plane active working set (plane $k-1$, plane $k$, plane $k+1$) is:
    $$3 \times 2.097\text{ MB} = 6.291\text{ MB}$$
  - **L2 Eviction**: Because $6.291\text{ MB} > 1.25\text{ MB}$ (L2 capacity by $5\times$), the previous $Z$-plane cannot stay in L2 cache as the solver iterates through an entire plane.
  - **L3 Pressure**: While 6.29 MB fits within the 18 MB shared L3 cache, each plane streaming traversal pushes 2.1 MB through cache lines, creating high L3 miss/traffic pressure and DRAM re-fetch demands if lines are evicted by cache associativity limits and write-allocate traffic from $u_{\text{new}}$.

### 3. Concrete Prediction
- **Target Optimization**: 3D spatial cache blocking along $Y$ and/or $Z$ (e.g. $B_z \times B_y$ blocks with full unit-stride row $X$) to keep the active sub-plane footprint below the $1.25\text{ MB}$ L2 cache limit.
- **Expected Speedup**: Unlike the 2D stencil where reuse was already satisfied by L2, 3D plane reuse is genuinely capacity-bound. We predict a measurable performance improvement of **$10\%\text{ to }25\%$** on the 3D step kernel when sub-plane tiles fit into L2 cache, assuming hardware prefetchers do not fully saturate DRAM bandwidth during sequential streaming.
