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

*(Note: Pre-implementation analysis and bounded prediction prior to any code change).*

### 1. Memory and Hardware Hierarchy Context
- **Domain Dimensions**: $512 \times 512 \times 512$ interior cells ($134,217,728$ points).
- **Memory Footprint**:
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

### 2. Theoretical Analysis of 3D Stencil Reuse
- **Plane Size**: A single $XY$-plane of doubles is $514 \times 514 \times 8\text{ bytes} \approx 2.11\text{ MB}$.
- **Active 4-Plane Working Set**:
  - To update row $(i, j, k)$, the stencil reads three input planes of $u_{\text{old}}$ (planes $k-1, k, k+1$) and writes to the destination plane of $u_{\text{new}}$ (plane $k$).
  - Total active 4-plane working set:
    $$3 \text{ read planes } (3 \times 2.11\text{ MB} = 6.33\text{ MB}) + 1 \text{ write plane } (2.11\text{ MB}) = \mathbf{8.44\text{ MB}}$$
  - **L2 Cache Eviction**: $8.44\text{ MB} \gg 1.25\text{ MB}$ (exceeding L2 capacity by $\approx 6.75\times$). During unblocked sequential traversal through plane $k$, earlier cells of planes $k-1, k,$ and $k+1$ are evicted from L2 cache.
  - **L3 Plane Survival Expectation**: The total grid size ($2.17\text{ GB}$) vastly exceeds the $18\text{ MB}$ L3 cache, so L3 holds only a dynamic window of the grids. The primary prediction assumes L3 retention of the active $8.44\text{ MB}$ plane slice between pass $k$ and pass $k+1$ ($8.44\text{ MB} < 18\text{ MB}$). Under this assumption, the gain stems from converting L3 hits into L2 hits (saving L3 latency and L3-to-L2 bus bandwidth). Alternatively, if associativity conflict misses or write-allocate thrashing evict lines to DRAM, unblocked misses incur DRAM latency (~175 cycles), yielding a separate conditional bound derived below.

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
- **Estimated Inputs & Bounded Gain Calculation**:
  - We model total execution time as $T = T_{\text{compute}} + T_{\text{stall}}$, where memory access stalls account for an estimated fraction $f_{\text{stall}} = T_{\text{stall}} / T \approx 0.20\text{ to }0.30$ (assumed input based on arithmetic-light stencil rooflines on x86 cores).
  - Unblocked execution incurs L2 capacity misses serviced by L3 (assumed latency ratio $L_{\text{L2}} / L_{\text{L3}} \approx 12\text{ cycles} / 35\text{ cycles} \approx 0.343$, giving an assumed latency reduction fraction $\Delta_{\text{latency}} = 1 - 12/35 \approx 0.657 \approx 0.66$).
  - Assuming spatial blocking eliminates L2 capacity misses for planes $k-1$ and $k$ (which comprise half the read streams, factor $s_{\text{reuse}} \approx 0.5$ to $0.7$), the expected fraction of runtime saved is bounded by:
    $$\text{Gain} \approx f_{\text{stall}} \times s_{\text{reuse}} \times \Delta_{\text{latency}}$$
    - Lower bound: $0.20 \times 0.50 \times 0.657 = 0.0657 \implies \mathbf{6.6\%}$
    - Upper bound: $0.30 \times 0.70 \times 0.657 = 0.1380 \implies \mathbf{13.9\%}$
  - This calculation yields the bounded prediction of **$6.6\%\text{ to }13.9\%$** for the L3-retention case.
  - **Conditional Bound (DRAM-Eviction Case)**:
    - If unblocked traversal fails to retain planes in L3 and spills to DRAM, we recompute with DRAM latency (~175 cycles):
      $$\Delta_{\text{latency, DRAM}} = 1 - (L_{\text{L2}} / L_{\text{DRAM}}) \approx 1 - (12 / 175) \approx 0.931 \approx 0.93$$
    - With the same assumed stall ($0.20\text{ to }0.30$) and reuse ($0.50\text{ to }0.70$) fractions:
      - Lower bound (DRAM): $0.20 \times 0.50 \times 0.931 = 0.0931 \implies \mathbf{9.3\%}$
      - Upper bound (DRAM): $0.30 \times 0.70 \times 0.931 = 0.1955 \implies \mathbf{19.5\%}$
    - Thus, if unblocked misses reach DRAM, the conditional predicted range is **$9.3\%\text{ to }19.5\%$**.
  - **Decision Rule & Interleaved Protocol Specification (Pre-Committed)**:
    - The evaluation of 3D spatial cache blocking ($B_z = 32, B_y = 32$) is strictly governed by a 20-pair interleaved A/B benchmark (40 runs total) with alternating lead order (10 pairs baseline-first, 10 pairs tiled-first) to eliminate both thermal drift and first-run cache advantage.
    - **Primary Decision Rule (Option A - Strict Paired Sign Test)**:
      - Fixed alternation schedule: Odd pairs $i \in \{1, 3, \dots, 19\}$ are Baseline-lead (Baseline $\to$ Tiled); even pairs $i \in \{2, 4, \dots, 20\}$ are Tiled-lead (Tiled $\to$ Baseline).
      - For each interleaved pair $i \in [1, 20]$, define the paired difference:
        $$\Delta_i = T_{\text{tiled}, i} - T_{\text{baseline}, i}$$
      - **Measured Improvement Criterion**: A measured improvement is claimed if and only if **every** paired difference is strictly negative:
        $$\Delta_{\text{max}} = \max_{i=1}^{20} \Delta_i < 0$$
      - If even a single pair exhibits $\Delta_i \ge 0$ (i.e. $\Delta_{\text{max}} \ge 0$), the outcome is formally and strictly classified as **no measurable difference**.
      - *Conservatism Note*: Option A is an intentionally conservative test. A real but modest ~8% effect subject to random OS jitter could produce a single non-negative pair; classifying $\Delta_{\text{max}} \ge 0$ as "no measurable difference" indicates lack of statistical certainty under strict sign consistency, not proof of zero physical effect.
    - **Single Governing Test**: This paired test $\Delta_{\text{max}} < 0$ is the sole test deciding the optimization outcome. The earlier unpaired distribution band ($[22.78\text{ s}, 27.58\text{ s}]$) applies strictly to standalone sets and is superseded by this paired protocol. The preliminary 10-run uncoupled median (23.11 s) is fully superseded.
    - **Variance Finding Rule**: The baseline spread is $2.40\text{ s}$, establishing a variance threshold of $1.5\times = 3.60\text{ s}$. If the tiled spread exceeds $3.60\text{ s}$, it is reported strictly as "tiled spread exceeds 3.60 s" (without speculative physical attribution to hardware mechanisms).
  - **Caveat**: If hardware stream prefetchers already hide memory latency during sequential row traversal, the actual stall fraction $f_{\text{stall}}$ may be near zero, in which case the measured gain will be smaller or neutral.

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
- Reps 7, 8, and 9 were slower ($25.9\text{ s}$–$26.9\text{ s}$); the cause is unexplained (possibly background OS scheduling or thermal variance across the continuous ~4.5-minute run). The median ($25.18\text{ s}$) represents the baseline reference.

---

### 5. Final 20-Pair Interleaved A/B Benchmark Results (Governing Outcome)

Measured on `configs/large_3d.cfg` ($512^3$, 50 steps, Dirichlet sine mode, 1R1T, Core 0 pinned) under a 20-pair balanced interleaved protocol (40 runs total) with alternating lead order, preceded by 1 warmup per configuration (`bench/benchmark_g4_3d_interleaved.csv`).

#### Paired Differences ($\Delta_i = T_{\text{tiled}, i} - T_{\text{baseline}, i}$)
- **Pair 1** (Lead: Base ): Base $21.527140\text{ s}$, Tiled $20.821492\text{ s}$, $\Delta = -0.705648\text{ s}$ ($-3.28\%$)
- **Pair 2** (Lead: Tiled): Tiled $20.007825\text{ s}$, Base $21.420817\text{ s}$, $\Delta = -1.412992\text{ s}$ ($-6.60\%$)
- **Pair 3** (Lead: Base ): Base $21.534446\text{ s}$, Tiled $20.772313\text{ s}$, $\Delta = -0.762133\text{ s}$ ($-3.54\%$)
- **Pair 4** (Lead: Tiled): Tiled $20.283392\text{ s}$, Base $22.011708\text{ s}$, $\Delta = -1.728316\text{ s}$ ($-7.85\%$)
- **Pair 5** (Lead: Base ): Base $21.483591\text{ s}$, Tiled $20.311378\text{ s}$, $\Delta = -1.172213\text{ s}$ ($-5.46\%$)
- **Pair 6** (Lead: Tiled): Tiled $20.658354\text{ s}$, Base $21.645626\text{ s}$, $\Delta = -0.987272\text{ s}$ ($-4.56\%$)
- **Pair 7** (Lead: Base ): Base $21.873985\text{ s}$, Tiled $20.174827\text{ s}$, $\Delta = -1.699158\text{ s}$ ($-7.77\%$)
- **Pair 8** (Lead: Tiled): Tiled $20.122086\text{ s}$, Base $28.483482\text{ s}$, $\Delta = -8.361396\text{ s}$ ($-29.36\%$)
- **Pair 9** (Lead: Base ): Base $21.663831\text{ s}$, Tiled $20.188202\text{ s}$, $\Delta = -1.475629\text{ s}$ ($-6.81\%$)
- **Pair 10** (Lead: Tiled): Tiled $20.052389\text{ s}$, Base $21.594569\text{ s}$, $\Delta = -1.542180\text{ s}$ ($-7.14\%$)
- **Pair 11** (Lead: Base ): Base $21.098109\text{ s}$, Tiled $19.786832\text{ s}$, $\Delta = -1.311277\text{ s}$ ($-6.22\%$)
- **Pair 12** (Lead: Tiled): Tiled $19.714942\text{ s}$, Base $20.762591\text{ s}$, $\Delta = -1.047649\text{ s}$ ($-5.05\%$)
- **Pair 13** (Lead: Base ): Base $20.621744\text{ s}$, Tiled $19.454883\text{ s}$, $\Delta = -1.166861\text{ s}$ ($-5.66\%$)
- **Pair 14** (Lead: Tiled): Tiled $19.800907\text{ s}$, Base $21.166099\text{ s}$, $\Delta = -1.365192\text{ s}$ ($-6.45\%$)
- **Pair 15** (Lead: Base ): Base $20.725155\text{ s}$, Tiled $19.545066\text{ s}$, $\Delta = -1.180089\text{ s}$ ($-5.69\%$)
- **Pair 16** (Lead: Tiled): Tiled $19.155744\text{ s}$, Base $20.746546\text{ s}$, $\Delta = -1.590802\text{ s}$ ($-7.67\%$)
- **Pair 17** (Lead: Base ): Base $20.980297\text{ s}$, Tiled $19.429931\text{ s}$, $\Delta = -1.550366\text{ s}$ ($-7.39\%$)
- **Pair 18** (Lead: Tiled): Tiled $19.466316\text{ s}$, Base $20.838748\text{ s}$, $\Delta = -1.372432\text{ s}$ ($-6.59\%$)
- **Pair 19** (Lead: Base ): Base $21.023001\text{ s}$, Tiled $19.699769\text{ s}$, $\Delta = -1.323232\text{ s}$ ($-6.29\%$)
- **Pair 20** (Lead: Tiled): Tiled $20.087377\text{ s}$, Base $20.982975\text{ s}$, $\Delta = -0.895598\text{ s}$ ($-4.27\%$)

#### Statistical Summary
- **$\Delta_{\text{max}}$ (Least Negative Difference)**: **$-0.705648\text{ s}$** (Pair 1)
- **$\Delta_{\text{min}}$ (Most Negative Difference)**: **$-8.361396\text{ s}$** (Pair 8, where Baseline suffered an isolated $28.48\text{ s}$ slow run; across the remaining 19 pairs, $\Delta_{\text{min}} = -1.728316\text{ s}$)
- **Median Paired Difference ($\\text{Median}(\Delta)$)**: **$-1.344212\text{ s}$**
- **In-Session Medians**:
  - Baseline In-Session Median: $21.293458\text{ s}$
  - Tiled In-Session Median: $20.030107\text{ s}$
  - Median Relative Gain: **$-6.31\%$**
- **Lead-Order Split Medians (Descriptive Only)**:
  - Baseline-lead pairs ($N=10$, odd pairs): Median $\Delta = -1.245683\text{ s}$ (range: $[-1.699\text{ s}, -0.706\text{ s}]$)
  - Tiled-lead pairs ($N=10$, even pairs): Median $\Delta = -1.392712\text{ s}$ (range: $[-8.361\text{ s}, -0.896\text{ s}]$)
  - *Lead-Order Qualification*: The split medians ($-1.25\text{ s}$ vs $-1.39\text{ s}$) are descriptive only. They demonstrate that both subsets independently yield negative median differences, but this descriptive similarity does not by itself formally exclude order or carry-over effects between runs.

---

### 6. Default Path Verification Against Approved G3 Base (49e68a1)

- **Pre-Stated Decision Rule for Default Path Equivalence**:
  - Binary A: Built from approved commit `49e68a1` with Release preset (`-O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic -ffp-contract=off -Werror -fopenmp`).
  - Binary B: Built from optimization commit `07ae288` with identical Release preset and compiler flags.
  - Test configuration: `configs/large_3d.cfg` ($512^3$, 50 steps, Core 0 pinned), running default unblocked execution ($B_z = 0, B_y = 0$).
  - Pre-run protocol: Dedicated warmup run executed for each binary prior to pair 1 to prevent cold-start cache/page-table bias on $\delta_1$.
  - **Decision Criterion**:
    - For each interleaved pair $i \in [1, 10]$, define $\delta_i = T_{07ae288, i} - T_{49e68a1, i}$.
    - Equivalence condition: Both of the following two explicit conditions must hold simultaneously:
      1. Zero inclusion in difference spread: $\delta_{\text{min}} \le 0 \le \delta_{\text{max}}$ (differences do not exhibit strict sign bias).
      2. Median bound: $|\text{Median}(\delta)| / \text{Median}(T_{49e68a1}) \le 2.0\%$ (median shift is within $\pm 2.0\%$).
    - If both conditions hold, the default path is formally classified as **no measurable difference** (consistent with unchanged performance).
    - *Methodological Caveat*: Given run-to-run variance, a 10-pair spread $\delta_{\text{min}} \le 0 \le \delta_{\text{max}}$ is a weak test that can include zero by chance; the 95% bootstrap confidence interval of the median difference is reported alongside it to characterize the bound on any undetectable shift.
- **Disassembly Audit**:
  - Extracted floating-point instructions from `step3DNoOverlap._omp_fn.0`:
    - Total FP instructions in inner stencil: **45 instructions** in both binaries.
    - Breakdown (verified by mnemonic extraction):
      - Scalar arithmetic: **26 instructions** (14 `addsd`, 6 `subsd`, 6 `mulsd`).
      - Register copies: **6 instructions** (6 `movapd`).
      - Memory load/store moves: **13 instructions** (12 scalar loads `movsd xmm, [mem]`, 1 store `movsd [mem], xmm0`).
  - Disassembly Diff: Normalizing register names (`r9`, `r13`, `rax`, `rbx`) and jump targets, the 45-instruction sequence of mathematical operations is identical.
- **Outlier and Sign-Test Invariance**: Pair 8's baseline run ($28.48\text{ s}$) drove $\Delta_{\text{min}}$, but because it was negative, it did not alter the sign test: all 20/20 pairs remained strictly negative.
- **Spread Comparison**:
  - Baseline Spread: $7.861738\text{ s}$ (or $1.389964\text{ s}$ across the other 19 runs)
  - Tiled Spread: **$1.665748\text{ s}$** (Min: $19.155744\text{ s}$, Max: $20.821492\text{ s}$)
  - The tiled spread of $1.67\text{ s}$ did not exceed the $3.60\text{ s}$ threshold.

#### Analytical Prediction Comparison & Honest Caveats
1. **Prediction Comparison**: The measured median gain of **$6.31\%$** is below the predicted range of **$6.6\%\text{ to }13.9\%$** (derived under the L3 retention assumption) and therefore falls just outside the predicted lower bound. The observed reduction is close to the analytical bound, but slightly under it.
2. **Cross-Session System Drift**: The unblocked baseline median shifted from $25.18\text{ s}$ in the earlier standalone session to $21.29\text{ s}$ in this interleaved session (a $15\%$ shift on identical code). While alternating interleaving controls for drift within a session, absolute wall times are not directly comparable across sessions. The earlier $25.18\text{ s}$ median and $2.40\text{ s}$ spread are not representative references for this session, and the $3.60\text{ s}$ threshold derived from them is a weak cross-session comparison.
3. **Telemetry & Frequency Limitation**: CPU frequency in `/proc/cpuinfo` reported a static `3110.402 MHz` across all runs, but virtualized cpufreq reporting in WSL2 does not measure physical core frequency directly and cannot verify whether hardware frequency scaling occurred. Average load averages were symmetric ($0.98$ baseline vs $0.99$ tiled).
4. **Governing Outcome**: **Improvement claimed under Option A** ($\\Delta_{\text{max}} = -0.705648\text{ s} < 0$, 20/20 pairs strictly negative).
5. **Scope of Claim**: This measured improvement applies strictly to the tested configuration ($512^3$, 50 steps, Dirichlet sine, 1 OpenMP thread pinned to Core 0, single session, tile shape $B_z=32, B_y=32$). Performance improvements beyond this tested scope are not established.
