# Viva Voce Preparation Guide

This document prepares the group for common viva questions on parallel computing, numerical stencils, MPI/OpenMP interaction, and performance engineering.

---

### Question 1: What numerical method is used and what determines its stability?
**Answer**: We use the explicit Forward-Time Central-Space (FTCS) finite-difference scheme. Its stability is governed by the CFL condition for diffusion:
$$\Delta t \le \frac{\Delta x^2}{2 \cdot d \cdot \alpha}$$
where $d$ is domain dimension and $\alpha$ is thermal diffusivity. If $\Delta t$ exceeds this threshold, high-frequency numerical errors grow exponentially without bound.
- **Code verification**: `heat::Config::isStable()` and `heat::Config::maxStableDt()` in `include/heat/config.hpp` and `src/config.cpp`.

### Question 2: Why choose a 2D Cartesian process grid over a 1D slab decomposition?
**Answer**: Surface-to-volume ratio. For $N \times N$ cells and $P$ processes:
- 1D slab: Each rank exchanges $2 \times N$ elements per time step. Total boundary communication surface is $2 N$.
- 2D grid: Each rank has $\approx (N/\sqrt{P}) \times (N/\sqrt{P})$ cells. Communication per rank is $4 \times (N/\sqrt{P})$.
For large $P$, $4 N / \sqrt{P} \ll 2 N$, drastically reducing halo message size per process.
- **Design reference**: `docs/design.md`, Section 2.

### Question 3: How does compute-communication overlap work in your solver?
**Answer**: Non-blocking `MPI_Irecv` and `MPI_Isend` are initiated for halo boundaries. The domain is logically split into an "interior" (cells at distance $\ge 2$ from ghost boundary) and an "exterior shell" (cells at distance 1). The CPU updates the interior while network interfaces (NIC) handle halo communication concurrently. We call `MPI_Waitall` and then update the shell.
- **Code verification**: Switchable via `--overlap` in the solver loop.

### Question 4: What is first-touch page placement and why does it matter for OpenMP?
**Answer**: Modern multi-socket CPUs utilize Non-Uniform Memory Access (NUMA). Operating systems assign physical memory pages to the NUMA node of the thread that first writes to that page. By initializing grid memory inside an OpenMP parallel loop with the same thread scheduling (`schedule(static)`) as the solver kernel, memory pages are placed in the NUMA node local to the thread that will process them, avoiding remote memory bus bottlenecks.

### Question 5: Why is the stencil computation memory-bound rather than compute-bound?
**Answer**: In 2D, updating each cell requires 1 read of center, 4 reads of neighbors (5 loads), 1 store, and 6 FLOPs (4 additions, 1 multiply, 1 assign/update). This gives an arithmetic intensity of $\approx 6 / (6 \times 8 \text{ bytes}) \approx 0.125 \text{ FLOPs/byte}$, far below modern processor balance points ($\approx 5\text{--}15 \text{ FLOPs/byte}$), making it heavily bandwidth-bound.

### Question 6: How do you know the parallel code is correct, not just fast?
**Answer**: Per-cell results are compared with the serial solver and must be exactly equal (`Max Diff = 0`), on uneven grids (67x53, 37x29x31) for 1, 2, 3, 4, 6 and 8 ranks, with overlap on and off. Each cell's update uses the same operations in the same order wherever it lives, and `-ffp-contract=off` stops the compiler fusing multiply-adds differently in different code paths. A halo-corruption mutation test proves the comparison can fail. Energy (a global sum) uses a 1e-12 relative tolerance because summation order differs.
- **Code**: `tests/test_mpi.cpp`, `docs/design.md` section 5.

### Question 7: Your speedup is only 2.8x on 4 cores. Is something wrong?
**Answer**: No, it is the expected behaviour of a bandwidth-bound kernel. The stencil does about 0.125 flop per byte, and the 4 cores share one memory system, so more cores cannot be fed proportionally. 3D reaches 3.22x (80.6%) because blocking improves reuse. We state this as the explanation consistent with the data; we did not read hardware bandwidth counters.
- **Data**: `results/g5/g5_summary.md`, `analysis/figures/g5_strong_2d.png`.

### Question 8: Why is pure MPI faster than OpenMP here, and when would hybrid win?
**Answer**: At 4 cores, 2D: MPI 2.80x vs OpenMP 1.82x; 3D: 3.22x vs 1.50x. Each rank sweeps a private compact subdomain; the threaded run has one large sweep and a barrier every step. Hybrid (2x2) sits between. Hybrid should help with many cores per node or many nodes, because fewer ranks mean fewer and larger halos and less MPI state. This machine has one socket and 4 usable cores, so we do not claim a hybrid win.

### Question 9: Did compute/communication overlap help?
**Answer**: No measurable benefit. We pre-registered the rule before running: improvement only if the median paired difference is negative and the 95% bootstrap CI excludes zero, using 10 interleaved pairs with balanced lead order. 2D: -2.5%, CI [-65, +38] ms. 3D: +2.9%, CI [-72, +373] ms. On one node, halos travel through shared memory, so there is little latency to hide. We report the null result rather than a favourable cherry-pick.

### Question 10: How does checkpoint/restart work and why is it exact?
**Answer**: Each rank writes its interior cells plus a header (dimension, grid, rank count, local extents, step, time) to a temporary file and renames it atomically. On restart every rank validates the header and all ranks must agree on the step, otherwise the run aborts on all ranks. Ghost cells are rebuilt by the first halo exchange, and the explicit scheme depends only on the previous step, so the continuation is bit-identical. Limitation: same rank count and grid are required.
- **Code**: `src/io.cpp`; tests `MpiIoTest.RestartIsBitIdentical2D/3D`, `RestartRejectsMismatchedGrid`.

### Question 11: How is the output visualised and why per-rank files?
**Answer**: Each rank writes its own VTK ImageData piece (cell data, raw binary), rank 0 writes a `.pvti` master and a `.pvd` time series. No gather is needed, so output scales with ranks and memory. `heat.pvd` opens directly in ParaView; `analysis/visualize.py` renders the same files to PNG/GIF.

### Question 12: What does your proposal say versus what you built?
**Answer**: The proposal named a 1D row-block decomposition, 8192x8192 for 5000 steps, and 3D as a stretch goal. We built a general 2D/3D Cartesian decomposition (less halo data per rank at higher rank counts), implemented 3D, and also ran the proposal's exact size once per configuration (`results/proposal_size.csv`). We did not benchmark 1D vs 2D decomposition against each other.
