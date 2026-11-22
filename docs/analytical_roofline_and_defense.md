# Analytical Roofline Modeling, Numerical Stability Derivations, and Viva Defense Framework

**Course**: Parallel and Distributed Computing (PDC), Section BCS-7A  
**Authors**: Sidhart Sami (23I-2527), Abdul Rafy (23P-0560)  
**Project**: Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)

---

## 1. Formal Mathematical Derivation of Von Neumann Stability

The explicit Forward-Time Central-Space (FTCS) discretization of the parabolic heat diffusion equation:
$$\frac{\partial u}{\partial t} = \alpha \nabla^2 u$$
approximates the continuous differential operator on an orthogonal Cartesian lattice with spatial intervals $\Delta x_i$ and time increment $\Delta t$.

### 1.1 Fourier Error Mode Decomposition
To evaluate numerical stability, we expand the discretization error at time step $n$ and spatial coordinate $\mathbf{x} = (j \Delta x, k \Delta y, l \Delta z)$ into orthogonal spatial Fourier modes:
$$\epsilon^n_{j,k,l} = \xi^n e^{I (k_x j \Delta x + k_y k \Delta y + k_z l \Delta z)}$$
where $I = \sqrt{-1}$, $\mathbf{k} = (k_x, k_y, k_z)$ is the spatial wavevector, and $\xi$ is the complex amplification factor.

### 1.2 Amplification Factor for the 3D 7-Point Stencil
Substituting the Fourier mode into the explicit 7-point update formula:
$$\epsilon^{n+1}_{j,k,l} = \epsilon^n_{j,k,l} + r_x (\epsilon^n_{j+1,k,l} - 2\epsilon^n_{j,k,l} + \epsilon^n_{j-1,k,l}) + r_y (\epsilon^n_{j,k+1,l} - 2\epsilon^n_{j,k,l} + \epsilon^n_{j,k-1,l}) + r_z (\epsilon^n_{j,k,l+1} - 2\epsilon^n_{j,k,l} + \epsilon^n_{j,k,l-1})$$
where $r_x = \frac{\alpha \Delta t}{\Delta x^2}$, $r_y = \frac{\alpha \Delta t}{\Delta y^2}$, and $r_z = \frac{\alpha \Delta t}{\Delta z^2}$.

Dividing across by $\epsilon^n_{j,k,l}$:
$$\xi = 1 + r_x (e^{I k_x \Delta x} - 2 + e^{-I k_x \Delta x}) + r_y (e^{I k_y \Delta y} - 2 + e^{-I k_y \Delta y}) + r_z (e^{I k_z \Delta z} - 2 + e^{-I k_z \Delta z})$$

Applying Euler's identity ($e^{I \theta} + e^{-I \theta} - 2 = 2 \cos(\theta) - 2 = -4 \sin^2(\theta/2)$):
$$\xi = 1 - 4 \left[ r_x \sin^2\left(\frac{k_x \Delta x}{2}\right) + r_y \sin^2\left(\frac{k_y \Delta y}{2}\right) + r_z \sin^2\left(\frac{k_z \Delta z}{2}\right) \right]$$

### 1.3 Stability Criterion
Von Neumann stability requires that the amplification factor satisfies $|\xi| \le 1$ for all permissible spatial wave frequencies $\mathbf{k}$.
Since the bracketed term is strictly non-negative, $\xi \le 1$ is trivially satisfied. The lower bound $\xi \ge -1$ yields:
$$1 - 4 \sum_{i=1}^d r_i \sin^2\left(\frac{k_i \Delta x_i}{2}\right) \ge -1 \implies 4 \sum_{i=1}^d r_i \sin^2\left(\frac{k_i \Delta x_i}{2}\right) \le 2$$

In the worst-case high-frequency oscillatory mode ($\sin^2(\dots) = 1$ for all axes simultaneously):
$$\sum_{i=1}^d r_i \le \frac{1}{2} \iff \Delta t \le \frac{1}{2 \alpha \sum_{i=1}^d \frac{1}{\Delta x_i^2}}$$

- **2D Uniform Grid**: $\Delta t \le \frac{\Delta x^2}{4 \alpha} \implies r \le 0.25$
- **3D Uniform Grid**: $\Delta t \le \frac{\Delta x^2}{6 \alpha} \implies r \le 0.1667$

---

## 2. Theoretical Roofline & Arithmetic Intensity Analysis

### 2.1 Computational Work vs. Memory Traffic
For each interior cell update in the 2D 5-point explicit stencil:
1. **Memory Reads**: 5 double-precision floating-point loads ($u_{i,j}, u_{i+1,j}, u_{i-1,j}, u_{i,j+1}, u_{i,j-1}$).
   *(Note: under ideal streaming cache reuse across adjacent rows, the minimum compulsory DRAM traffic is 1 load and 1 store per updated cell)*.
2. **Memory Writes**: 1 double-precision store ($u^{\text{new}}_{i,j}$).
3. **Floating-Point Operations (FLOPs)**:
   - $u_{i+1,j} + u_{i-1,j} - 2 u_{i,j}$ (2 additions, 1 multiply)
   - $u_{i,j+1} + u_{i,j-1} - 2 u_{i,j}$ (2 additions, 1 multiply)
   - Weighted sum and time advancement: $\approx 6 \text{ FLOPs}$ total per cell.

### 2.2 Operational Intensity Calculation
- **Compulsory DRAM Traffic (Worst Case / No Cache Reuse)**:
  $$\text{Traffic} = (5 \text{ loads} + 1 \text{ store}) \times 8 \text{ bytes} = 48 \text{ bytes}$$
  $$\text{Arithmetic Intensity } (I) = \frac{6 \text{ FLOPs}}{48 \text{ bytes}} = 0.125 \text{ FLOPs/byte}$$
- **Compulsory DRAM Traffic (Optimal Temporal/Spatial Cache Reuse)**:
  $$\text{Traffic}_{\text{opt}} = (1 \text{ compulsory load} + 1 \text{ compulsory write}) \times 8 \text{ bytes} = 16 \text{ bytes}$$
  $$I_{\text{opt}} = \frac{6 \text{ FLOPs}}{16 \text{ bytes}} = 0.375 \text{ FLOPs/byte}$$

### 2.3 Hardware Machine Balance Comparison
- **Intel Core i5-12500H P-core Baseline**:
  - Peak Compute Throughput (AVX2, 4 Cores @ 3.5 GHz): $\approx 448 \text{ GFLOPs/s}$.
  - Dual-Channel DDR4/DDR5 Memory Bandwidth Ceiling: $\approx 50\text{--}60 \text{ GB/s}$.
  - Machine Balance ($\beta$): $\frac{448 \text{ GFLOPs/s}}{55 \text{ GB/s}} \approx 8.15 \text{ FLOPs/byte}$.

$$\text{Kernel Arithmetic Intensity } (0.125\text{--}0.375 \text{ FLOPs/byte}) \ll \text{Machine Balance } (8.15 \text{ FLOPs/byte})$$

**Conclusion**: The heat equation stencil is overwhelmingly **memory-bandwidth bound**. The compute execution pipeline spends the majority of its cycle budget stalled awaiting cache lines from DRAM. Adding physical compute cores beyond memory saturation yields diminishing returns, mathematically explaining the sub-linear parallel scaling efficiencies (70.0% in 2D, 80.6% in 3D).

---

## 3. Communication-Computation Overlap Dynamics

### 3.1 Overlap State Machine
The asynchronous overlap mechanism divides the local grid $\Omega_{\text{local}}$ into two disjoint spatial subsets:
1. **Interior Domain ($\Omega_{\text{int}}$)**: Cells at Euclidean distance $\ge 2$ from any partition boundary face. Updates depend strictly on step $n$ values owned locally.
2. **Boundary Shell ($\Omega_{\text{shell}}$)**: Cells at distance 1 from partition boundaries whose update requires ghost values from adjacent MPI ranks.

```
       [ Ghost Cells (MPI Received) ]
----------------------------------------------
|          Boundary Shell (Shell)            | <- Updated after MPI_Waitall
|   --------------------------------------   |
|   |                                    |   |
|   |         Interior Domain            |   | <- Concurrently updated while
|   |          (Interior)                |   |    MPI_Isend/Irecv are in-flight
|   |                                    |   |
|   --------------------------------------   |
|          Boundary Shell (Shell)            |
----------------------------------------------
```

### 3.2 Quantitative Explanation of the Null Result on Single-Socket Hosts
In our pre-registered Gate 5 experiment:
- 2D $4096^2$: Overlap delta $-2.5\%$, 95% bootstrap CI $[-65, +38]\text{ ms}$ (includes 0).
- 3D $512^3$: Overlap delta $+2.9\%$, 95% bootstrap CI $[-72, +373]\text{ ms}$ (includes 0).

**Physical Mechanism**:
1. **Zero Network Latency**: In single-host execution, OpenMPI employs the shared-memory BTL (`vader` / cross-memory attach `cma`). Transfer latency is bounded by L3 cache interconnect speeds ($\approx 0.1\text{--}0.5 \mu\text{s}$), leaving virtually no idle latency to hide.
2. **Loop Partitioning Overhead**: Splitting a single contiguous nested loop traversal into an interior block plus multiple boundary planar traversals disrupts compiler software pipelining and hardware branch prediction.
3. **Future Scaling Advantage**: Compute-communication overlap becomes beneficial on multi-node InfiniBand/Ethernet clusters where message transit latency exceeds $5\text{--}50 \mu\text{s}$.

---

## 4. Key Viva Voce Technical Defense Strategies

| Question Area | Core Technical Defense |
|---|---|
| **Why not 1D slab decomposition?** | A 2D Cartesian grid minimizes the surface-to-volume ratio ($4 N / \sqrt{P} \ll 2 N$ for large $P$), reducing total message footprint and network buffer contention. |
| **Why is serial vs. parallel bit-identical?** | Each cell evaluated in step $n+1$ is an invariant algebraic combination of step $n$ values. Because double-buffering isolates steps and `-ffp-contract=off` prevents floating-point contraction differences, output is exact bit-for-bit (`Max Diff = 0.0`). |
| **Why did pure MPI beat OpenMP?** | Each MPI rank sweeps a private, compact contiguous domain with independent NUMA first-touch pages and private cache working sets. OpenMP introduces barrier synchronization overhead across all threads at every step. |
| **How does checkpoint/restart ensure fault tolerance?** | Atomic filesystem rename prevents partial file corruption. Header extents are validated across all ranks, and step numbers must match collectively, allowing bit-identical resumption of interrupted computations. |
