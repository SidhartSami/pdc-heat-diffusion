# 1. Selection of C++17 Standard

Date: 2026-10-22

## Context and Problem Statement
The project requires a modern, clean, robust, and portable language dialect for high-performance computing using both MPI and OpenMP. We need to decide whether to use C++17 or C++20.

## Decision Drivers
- High portability across university HPC clusters, CI runners, and local developer environments.
- First-class support in standard MPI wrappers (`mpicxx`) and OpenMP implementations without requiring experimental compiler flags.
- Modern C++ abstractions (structured bindings, `std::string_view`, `std::optional`, `if constexpr`) that improve readability and maintainability without incurring runtime overhead.

## Considered Options
1. C++17
2. C++20

## Decision Outcome
Chosen option: **C++17**, because:
- C++17 is universally supported across GCC (>= 8), Clang (>= 7), and standard HPC vendor toolchains.
- C++20 modules and concepts are not yet universally supported across all MPI header implementations without friction.
- C++17 provides all necessary modern features (`constexpr`, structured bindings, `std::byte`, `<filesystem>`).

## Consequences
- Positive: Stable compilation across GCC, Clang, Intel, and MSVC with zero friction with OpenMPI and OpenMP.
- Positive: Guaranteed compatibility in Ubuntu CI and HPC cluster environments.
- Negative: Cannot use C++20 ranges or concepts natively (though standard template SFINAE or static assertions suffice for our dimension-aware templates).
