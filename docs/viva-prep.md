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
