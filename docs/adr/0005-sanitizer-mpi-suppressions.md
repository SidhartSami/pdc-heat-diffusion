# 5. OpenMPI Internal Runtime Memory & Hook Suppressions under Sanitizers

Date: 2026-10-22

## Context and Problem Statement
When running AddressSanitizer (ASan) and LeakSanitizer (LSan) on applications initialized with `MPI_Init`, OpenMPI's internal modular component architecture (MCA) components (`libopen-pal`, `libpmix`, `libevent`, `libuct_rocm`) dynamically allocate state during initialization that persists across process lifetime or relies on function hooking (`patcher:overwrite`).
Specifically:
1. `mca_base_patcher_patch_apply_binary` in `libopen-pal` attempts runtime binary text patching which triggers an access violation (SEGV) under ASan unless `OMPI_MCA_patcher=^overwrite` is set.
2. Internal global structures in PMIx, libevent, and ROCm/UCX (`pmix_rte_init`, `uct_rocm_base_init`) are not completely freed during `MPI_Finalize` by design in OpenMPI, leading to reported leaks in `/usr/lib/x86_64-linux-gnu/libopen-pal.so`, `libpmix.so`, and `libmpi.so`. None of these allocations originate from our heat diffusion solver.

## Decision Drivers
- Zero tolerance for leaks or errors in project code (`include/heat/`, `src/`, `tests/`).
- Ability to run ASan and LSan cleanly across MPI tests in CI and locally without being blocked by third-party vendor library one-time runtime allocations.

## Considered Options
1. Disable LeakSanitizer entirely on MPI tests.
2. Provide a standard LSan suppression file (`lsan_mpi.supp`) suppressing known leaks within `libopen-pal.so`, `libpmix.so`, `libmpi.so`, and `libuct*.so`, and setting `OMPI_MCA_patcher=^overwrite`.

## Decision Outcome
Chosen option: **Standard LSan suppression file (`lsan_mpi.supp`) and disabling MCA overwrite patcher in sanitizer test environments.**
- All project functions (`heat::*`) remain 100% strictly checked for memory leaks and memory safety.
- Only third-party OpenMPI runtime shared libraries are suppressed.

## Consequences
- Positive: Our test suite runs cleanly under AddressSanitizer and LeakSanitizer without false positives from external MPI runtime internals.
- Positive: Standard practice in HPC testing when using OpenMPI with LLVM/GCC sanitizers.
