# Distributed Memory Architecture, NUMA Topologies, and Execution State Machines

**Course**: Parallel and Distributed Computing (PDC), Section BCS-7A  
**Authors**: Sidhart Sami (23I-2527), Abdul Rafy (23P-0560)  
**Project**: Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

---

## 1. Domain Decomposition & Contiguous Memory Striding

To maximize hardware prefetcher efficiency and avoid heap fragmentation, both 2D and 3D computational domains are allocated as single, contiguous 1D memory buffers in double-precision floating point (`std::vector<double>`).

### 1.1 Memory Striding and Index Resolution
Each local subdomain incorporates a uniform 1-cell halo padding along all coordinate boundaries.

```
       +---------------------------------------------+
       |   Halo Cell   (y = ny + 1)                  |
       +---------------------------------------------+
       | H |                                     | H |
       | a |        Interior Physical Domain     | a |
       | l |        x in [1, nx], y in [1, ny]   | l |
       | o |                                     | o |
       +---------------------------------------------+
       |   Halo Cell   (y = 0)                       |
       +---------------------------------------------+
```

- **2D Grid Mapping**:
  $$\text{Index}_{2D}(x, y) = y \cdot (\text{local\_nx} + 2) + x$$
  Total elements allocated: $(\text{local\_nx} + 2) \times (\text{local\_ny} + 2)$
- **3D Grid Mapping**:
  $$\text{Index}_{3D}(x, y, z) = z \cdot [(\text{local\_ny} + 2) \cdot (\text{local\_nx} + 2)] + y \cdot (\text{local\_nx} + 2) + x$$
  Total elements allocated: $(\text{local\_nx} + 2) \times (\text{local\_ny} + 2) \times (\text{local\_nz} + 2)$

### 1.2 Remainder Distribution across Cartesian Processes
When global grid dimensions $N$ are not evenly divisible by Cartesian grid dimensions $P$, boundary loads are equalized using a balanced remainder distribution formula:
$$\text{local\_n} = \left\lfloor \frac{N}{P} \right\rfloor + (r < (N \pmod P) \ ? \ 1 : 0)$$
$$\text{start} = \begin{cases} r \cdot (\lfloor N/P \rfloor + 1), & r < (N \pmod P) \\ (N \pmod P) \cdot (\lfloor N/P \rfloor + 1) + (r - (N \pmod P)) \cdot \lfloor N/P \rfloor, & r \ge (N \pmod P) \end{cases}$$
This prevents load imbalance skew from degrading Amdahl scaling limits.

---

## 2. Halo Exchange Buffer Staging & Communication Topology

```
                  Top Neighbor (MPI Rank N_top)
                             ▲
                             │  MPI_Isend / MPI_Irecv (Tag 102/103)
                             ▼
Left Neighbor     +---------------------+     Right Neighbor
(MPI Rank N_left) |   Local Subdomain   |     (MPI Rank N_right)
      ◄───────────┤   nx x ny Cells     ├───────────►
MPI_Isend/Irecv   +---------------------+   MPI_Isend/Irecv
(Tag 100/101)                ▲              (Tag 100/101)
                             │  MPI_Isend / MPI_Irecv (Tag 102/103)
                             ▼
                Bottom Neighbor (MPI Rank N_bottom)
```

### 2.1 Contiguous Pack/Unpack Buffer Strategy
Because row-major memory is non-contiguous along the $Y$ and $Z$ axes, attempting non-contiguous strided DMA can cause performance penalties in low-level network fabrics.
- **Dedicated Staging Vectors**: Pre-allocated contiguous vectors (`m_send_left`, `m_send_right`, etc.) hold serialized slices.
- **Packing Overhead**: Packing costs $\mathcal{O}(\text{surface})$ memory moves, which is negligibly small relative to $\mathcal{O}(\text{volume})$ stencil arithmetic for large subdomains.
- **Asynchronous Isolation**: Non-blocking requests (`MPI_Isend`, `MPI_Irecv`) reference isolated buffers, preventing data race hazards while OpenMP threads execute interior sweeps concurrently.

---

## 3. NUMA First-Touch Policy & Thread Affinity

Modern symmetric multiprocessing (SMP) and multi-socket platforms feature Non-Uniform Memory Access (NUMA). Memory access latency to local socket RAM is substantially lower than cross-socket interconnect latency.

```
+------------------------------------+------------------------------------+
|            NUMA Node 0             |            NUMA Node 1             |
|  +------------------------------+  |  +------------------------------+  |
|  |       Physical Core 0        |  |  |       Physical Core 2        |  |
|  |  [L1D: 48KB]  [L2: 1.25MB]   |  |  |  [L1D: 48KB]  [L2: 1.25MB]   |  |
|  +------------------------------+  |  +------------------------------+  |
|                 │                  |                 │                  |
|                 ▼                  |                 ▼                  |
|       Local NUMA DRAM Page         |       Local NUMA DRAM Page         |
|   (First-touched by Core 0)        |   (First-touched by Core 2)        |
+------------------------------------+------------------------------------+
```

### 3.1 Kernel Initialization Alignment
The Linux virtual memory subsystem binds physical DRAM pages to the NUMA node of the CPU core that executes the first write access to that virtual address (*first-touch* rule).
- If the master thread initializes the entire grid sequentially, 100% of memory pages map to a single NUMA bank, causing severe memory bus contention during multithreaded iterations.
- By matching OpenMP initialization scheduling (`#pragma omp parallel for schedule(static)`) identically to the solver computation loop, pages are guaranteed to reside in the memory bank local to the thread that updates them.

### 3.2 Alder Lake P-Core Pinning
On Intel 12th/13th Gen hybrid architectures:
- Benchmark execution is restricted to physical Performance cores (CPUs 0, 2, 4, 6) using `taskset -c 0,2,4,6`.
- OpenMP thread binding is enforced via:
  ```bash
  export OMP_PLACES=cores
  export OMP_PROC_BIND=close
  ```
  preventing OS scheduler thread migration and preserving L1/L2 cache residency across iterations.

---

## 4. Checkpoint / Restart Serialization Specification

To guarantee fault tolerance during multi-hour high-resolution simulations, checkpoint files are serialized per rank.

### 4.1 Binary Header Layout (60 Bytes)
```
Byte Offset   Field Name   Data Type   Description
0x00 - 0x03   dim          Int32       Spatial dimension (2 or 3)
0x04 - 0x07   nx           Int32       Global grid size in X
0x08 - 0x0B   ny           Int32       Global grid size in Y
0x0C - 0x0F   nz           Int32       Global grid size in Z
0x10 - 0x13   nranks       Int32       Total number of MPI ranks
0x14 - 0x17   rank         Int32       Calling rank ID
0x18 - 0x1B   start_x      Int32       Global starting coordinate X
0x1C - 0x1F   local_nx     Int32       Local grid length X
0x20 - 0x23   start_y      Int32       Global starting coordinate Y
0x24 - 0x27   local_ny     Int32       Local grid length Y
0x28 - 0x2B   start_z      Int32       Global starting coordinate Z
0x2C - 0x2F   local_nz     Int32       Local grid length Z
0x30 - 0x33   step         Int32       Completed simulation step
0x34 - 0x3B   t            Float64     Current physical simulation time
0x3C - ...    data[]       Float64[]   Contiguous raw interior cell array
```

### 4.2 Atomic Swap Protocol
1. Each rank writes to `<prefix>_r<rank>.bin.tmp`.
2. Flushes and closes file handles.
3. Invokes atomic POSIX filesystem rename (`std::filesystem::rename`) to `<prefix>_r<rank>.bin`.
4. Executes collective barrier to ensure global checkpoint consistency before advancing to step $n+1$.
