# Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

**Course**: Parallel & Distributed Computing (PDC), Section BCS-7A
**Authors**: Sidhart Sami (23I-2527), Abdul Rafy (23P-0560)
**Parallel model**: Hybrid distributed + shared memory (MPI + OpenMP)

## Abstract

We implement and evaluate a hybrid MPI + OpenMP solver for the 2D and 3D heat equation using the explicit
FTCS finite-difference scheme. The domain is decomposed over an MPI Cartesian communicator, halos are
exchanged with non-blocking point-to-point messages (optionally overlapped with interior computation),
and each rank's stencil sweep is threaded with OpenMP. The parallel result is **bit-identical** to the
serial result for 1-8 ranks, in 2D and 3D, with and without overlap. On a 4-core P-core allocation the
solver reaches **2.80x** (2D, 4096x4096) and **3.22x** (3D, 512^3) strong-scaling speedup with pure MPI;
the stencil is memory-bandwidth-bound, which explains the sub-linear efficiency. A pre-registered
interleaved experiment finds **no measurable benefit from communication/computation overlap on a single
shared-memory node**. The solver also writes ParaView-readable parallel VTK output and supports
bit-exact checkpoint/restart.

## 1. Problem and numerical method

We solve u_t = alpha * laplacian(u) on the unit square/cube with Dirichlet or zero-flux Neumann boundaries,
using FTCS with a 5-point (2D) or 7-point (3D) stencil. Stability requires
dt <= 1 / (2 * alpha * sum_i 1/dx_i^2); the solver refuses to run otherwise, and `dt=auto` selects 0.9x the
limit. Correctness was established in Gate 1 against the analytic sine-mode decay (O(dx^2) spatial
convergence) and by energy conservation under Neumann conditions.

Relation to the proposal: the proposal specified a 1D row-block decomposition on an 8192x8192 grid for 5000
steps, with 3D as a stretch goal. We implemented the more general 2D/3D Cartesian decomposition (smaller
halo surface per rank than row blocks at higher rank counts) and 3D, and we also ran the proposal's exact
problem size (Section 5.5).

## 2. Implementation

| Layer | Implementation |
|---|---|
| Decomposition | `MPI_Cart_create`/`MPI_Cart_shift`, uneven sizes via remainder distribution (`decomposition.cpp`) |
| Halo exchange | Contiguous pack/unpack buffers, `MPI_Isend`/`MPI_Irecv`/`MPI_Waitall` (ADR 0004) |
| Overlap | Post halo messages, update interior, wait, update boundary shell (`mpi_solver.cpp`) |
| Threading | `#pragma omp parallel for` over rows, first-touch initialisation, `OMP_PLACES=cores`, `OMP_PROC_BIND=close`, `MPI_THREAD_FUNNELED` |
| Cache blocking (3D) | Optional Bz x By tiling; measured -6.3% median on 512^3 (Gate 4 notes) |
| Output | One VTI piece per rank + PVTI master + PVD time series (`io.cpp`), no gather needed |
| Checkpoint/restart | Per-rank binary files, atomic rename, header validation, collective step agreement |

Design decisions are recorded in `docs/adr/0001-0005` and `docs/design.md`.

## 3. Verification

- 23 tests (GoogleTest): config/CLI, serial analytic and convergence tests, MPI tests at 1, 2, 3, 4, 6, 8 ranks.
- Serial vs. parallel: `Max Diff = 0` (exact) on uneven 2D (67x53) and 3D (37x29x31) grids, with overlap on and off.
- Halo tests encode global coordinates and check every ghost cell; a mutation test confirms a corrupted halo is detected.
- ASan/UBSan/LSan clean for 1-8 ranks (Gate 2); clang-tidy clean.
- **Restart**: a run interrupted at step N and restarted equals the uninterrupted run with `Max Diff = 0`, in 2D and 3D, at 1-8 ranks; a checkpoint from a different grid is rejected on every rank.
- **VTI output**: test reads each rank's piece back and compares to the gathered field exactly.

Why bit-identical: each cell's update depends only on the previous step and uses the same operations in the same order regardless of which rank owns it (`-ffp-contract=off` prevents fused-multiply-add differences across code paths). Global reductions (energy) are compared with a 1e-12 relative tolerance because summation order differs.

## 4. Experimental setup

Intel Core i5-12500H (Alder Lake: 4 P-cores / 8 threads + 8 E-cores), laptop under WSL2 (about 7 GB RAM visible to Linux), GCC 15.2, OpenMPI 5.0.10.
All scaling runs are pinned to physical P-cores without SMT sharing (logical CPUs 0, 2, 4, 6) with
`taskset` + `mpirun --bind-to core`; E-cores are excluded to avoid heterogeneous throttling. Protocol
(pre-registered in `docs/g5_scaling_plan.md`): 1 warmup + 7 recorded runs per configuration, median wall
time (slowest rank), reported with range and IQR; speedup S = T(1)/T(p), efficiency E = S/p. Overlap
comparisons use 10 interleaved pairs with balanced lead order and a 95% bootstrap CI of paired differences
(20000 resamples, fixed seed).

## 5. Results

All data: `results/g5/`; regenerate tables and figures with `python analysis/analyze_g5.py`.

### 5.1 Strong scaling, 2D 4096x4096, 100 steps (`analysis/figures/g5_strong_2d.png`)

| Config | Cores | Median (s) | Speedup | Efficiency |
|---|---|---|---|---|
| MPI P=1 | 1 | 3.126 | 1.00 | 100% |
| MPI P=2 | 2 | 1.824 | 1.71 | 85.7% |
| MPI P=4 | 4 | 1.117 | 2.80 | 70.0% |
| OpenMP T=2 | 2 | 1.997 | 1.57 | 78.3% |
| OpenMP T=4 | 4 | 1.720 | 1.82 | 45.4% |
| Hybrid 2x2 | 4 | 1.376 | 2.27 | 56.8% |

### 5.2 Strong scaling, 3D 512^3, 50 steps, Bz=By=32 (`g5_strong_3d.png`)

| Config | Cores | Median (s) | Speedup | Efficiency |
|---|---|---|---|---|
| MPI P=1 | 1 | 22.34 | 1.00 | 100% |
| MPI P=2 | 2 | 11.07 | 2.02 | 100.9% |
| MPI P=4 | 4 | 6.93 | 3.22 | 80.6% |
| OpenMP T=2 | 2 | 16.27 | 1.37 | 68.6% |
| OpenMP T=4 | 4 | 14.92 | 1.50 | 37.4% |
| Hybrid 2x2 | 4 | 9.21 | 2.42 | 60.6% |

The 100.9% at 2 ranks is inside the baseline's own spread (range 1.68 s) and is not claimed as superlinear scaling.

### 5.3 Weak scaling (`g5_weak_scaling.png`)

| Suite | P=1 | P=2 | P=4 |
|---|---|---|---|
| 2D, 2048^2 per core: time (s) / efficiency | 0.800 / 100% | 0.853 / 93.7% | 1.125 / 71.0% |
| 3D, 256^3 per core: time (s) / efficiency | 2.354 / 100% | 2.235 / 105.3% | 2.596 / 90.7% |

### 5.4 Overlap evaluation, P=4, interleaved pairs (`g5_overlap.png`)

| Case | Median overlap (s) | Median non-overlap (s) | Median delta | 95% CI of delta | Verdict |
|---|---|---|---|---|---|
| 2D 4096^2 | 1.135 | 1.153 | -2.5% | [-65, +38] ms | no measurable difference |
| 3D 512^3 | 6.808 | 6.593 | +2.9% | [-72, +373] ms | no measurable difference |

Pre-registered rule: improvement only if median delta < 0 and the CI excludes 0. Neither case qualifies.

### 5.5 Proposal-size run (8192x8192, 5000 steps)

The exact problem size from the proposal, 2D 8192x8192 (about 1.07 GB for two buffers), 5000 steps, one run
per configuration (`bench/run_proposal_size.sh`, `results/proposal_size.csv`). Single runs, so this shows the
sequential-vs-parallel gap but carries no confidence interval.

| Config | Cores | Wall time | Speedup | Efficiency |
|---|---|---|---|---|
| Sequential-equivalent (MPI P=1) | 1 | 452.6 s (7.5 min) | 1.00 | 100% |
| MPI P=4 | 4 | 184.8 s (3.1 min) | 2.45 | 61.2% |
| Hybrid P=2 x T=2 | 4 | 190.1 s (3.2 min) | 2.38 | 59.6% |

The sequential run is clearly slow (7.5 minutes) and 4 cores cut it to about 3 minutes, which is the
measurable gap the proposal asked for. Efficiency is lower than at 4096^2 (70%); a one-off run cannot tell
us whether that is the larger working set or ordinary variation. MPI-4 and hybrid differ by 3%, which is
within the spread seen elsewhere, so we do not rank them from this run.

## 6. Discussion

**Why efficiency is sub-linear.** The 5-point stencil performs ~6 flops per 6 memory accesses (arithmetic
intensity ~0.125 flop/byte), so it is limited by memory bandwidth, not compute. The 4 cores share one
memory system, so adding cores adds compute that the memory bus cannot feed; weak-scaling efficiency drops
for the same reason. We did not read bandwidth counters, so this is the explanation consistent with the data, not a measured proof.

**Ranks vs. threads.** Pure MPI beat pure OpenMP at equal core counts in both dimensions (2D 2.80x vs 1.82x;
3D 3.22x vs 1.50x). Each rank owns a private, compact subdomain with its own first-touched memory, while the
threaded version shares one large sweep per step with a barrier. Hybrid 2x2 lands between the two. On this
single-socket machine pure MPI is the best configuration; hybrid is expected to pay off when there are many
cores per NUMA domain or many nodes (fewer, larger halos).

**Why overlap did not help.** Overlap hides message latency behind computation. Ranks on one node exchange
halos through shared memory, so there is little latency to hide, and the interior/shell split adds loop
overhead. The measured null result is consistent with that. A benefit is plausible only on a real network.

**Small problems.** At 512x512 (about 0.1 s of work) 4 threads reach only 31.7% efficiency (Gate 3): fork/join and
barrier cost is comparable to the work. Efficiency improves with problem size.

## 7. Limitations (stated plainly)

- One machine, one session; scaling is measured only up to 4 cores (P-cores). No multi-node runs.
- WSL2 virtualises CPU frequency control, so per-run clock frequency was not sampled (load average was logged).
- Run-to-run spread is 3-10% of the median in several configurations (see range/IQR in `results/g5/g5_summary.md`); differences smaller than that should not be over-interpreted.
- The proposal's 1D row-block decomposition was replaced by a Cartesian one; we did not benchmark 1D vs 2D at equal rank count.
- Checkpoint restart requires the same rank count and grid; it does not re-partition.

## 8. Reproducing

```bash
cmake --preset release && cmake --build --preset release -j
ctest --preset release                       # 23 tests
./demo.sh                                    # correctness, speedup, visualization, restart
bench/run_g5_scaling.sh [A B C D E]          # scaling suites (about 20 min for all)
python analysis/analyze_g5.py                # tables + figures
python analysis/visualize.py output/demo_viz # animation of snapshots
```

## 9. References

See `docs/references.md` (LeVeque 2007; Gropp, Lusk, Skjellum 2014; MPI Standard 3.1/4.0; OpenMP specifications).
