# 1. Selection of C++17 Standard & Pure MPI C API

Date: 2026-10-22

## Context and Problem Statement
The project requires a modern, clean, robust, and portable language dialect for high-performance computing using both MPI and OpenMP. We need to decide whether to use C++17 or C++20, and which MPI interface standard to target.

## Decision Drivers
- High portability across university HPC clusters, CI runners, and local developer environments.
- First-class support in standard MPI wrappers (`mpicxx`) and OpenMP implementations without requiring experimental compiler flags.
- Modern C++ abstractions (structured bindings, `std::string_view`, `std::optional`, `if constexpr`) that improve readability and maintainability without incurring runtime overhead.
- Compliance with modern MPI standards: The legacy MPI C++ bindings (`<mpi.h>` C++ classes under namespace `MPI::`) were officially deprecated in MPI-2.2 and completely removed in MPI-3.0. Modern MPI implementations (such as OpenMPI 5.x) do not ship or support them.

## Considered Options
1. C++17 with standard MPI C API wrapped in RAII helpers
2. C++20 with standard MPI C API
3. Legacy MPI C++ bindings (Rejected: removed in MPI-3.0)

## Decision Outcome
Chosen option: **C++17 with the standard MPI C API (`#include <mpi.h>`)**, because:
- C++17 is universally supported across GCC (>= 8), Clang (>= 7), and standard HPC vendor toolchains.
- C++20 modules and concepts are not yet universally supported across all MPI headers and HPC environments without friction.
- C++17 provides all necessary modern features (`constexpr`, structured bindings, `std::byte`, `<filesystem>`).
- The standard MPI C API is the universal, authoritative, and supported MPI interface in MPI 3.x, 4.x, and 5.x implementations. All calls are checked via custom C++ error-handling macros/wrappers.

## Consequences
- Positive: Stable compilation across GCC, Clang, Intel, and MSVC with zero friction with OpenMPI and OpenMP.
- Positive: Guaranteed compatibility in Ubuntu CI and HPC cluster environments.
- Positive: Avoids deprecated or removed legacy MPI C++ interfaces.
- Negative: Cannot use C++20 ranges or concepts natively (standard template SFINAE or static assertions suffice for our dimension-aware templates).
