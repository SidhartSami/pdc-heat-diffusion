# 4. Halo Exchange Architecture: Contiguous Pack/Unpack Buffers vs Subarrays

Date: 2026-10-22

## Context and Problem Statement
In distributed-memory 2D and 3D domain decomposition, neighbor subdomains must exchange boundary halos at each time step. In C++ row-major arrays, only the contiguous X-dimension faces are naturally contiguous in memory. Slices in Y (strided) and Z (2D planes) are non-contiguous. We must choose how to exchange halo data.

## Decision Drivers
- Maximum portability across various MPI implementations without relying on implementation-specific optimizations in derived datatypes.
- Cache locality during communication packing/unpacking.
- Explicit control over memory buffers to facilitate compute-communication overlap.
- Clear, understandable code for examination viva defense.

## Considered Options
1. `MPI_Type_vector` / `MPI_Type_create_subarray` derived datatypes.
2. Contiguous pack/unpack buffers for non-contiguous faces combined with standard contiguous transfers (`MPI_DOUBLE`).

## Decision Outcome
Chosen option: **Contiguous pack/unpack buffers for non-contiguous faces (with direct contiguous transfers for X-faces where possible, or unified pack/unpack buffers)**:
- Contiguous send/receive buffers guarantee optimal network hardware (DMA/RDMA) transfer throughput without vendor-specific pack/unpack inefficiencies inside the MPI runtime.
- Decouples communication staging from stencil loop indexing, making compute-communication overlap straightforward to implement and verify.
- Easy to inspect and verify in unit tests (e.g., checking ghost cell encoding directly against neighbor ranks).

## Consequences
- Positive: Predictable zero-copy or single-copy memory packing behavior.
- Positive: Explicit buffer boundaries make asynchronous non-blocking (`MPI_Isend`/`MPI_Irecv`/`MPI_Waitall`) lifetime management transparent.
- Negative: Requires small local buffer allocations for halo faces (tiny relative to the 3D grid volume).
