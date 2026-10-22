# 2. Dependency-Free Key=Value Config File Parser

Date: 2026-10-22

## Context and Problem Statement
Every simulation and benchmark run must be configurable and reproducible from a configuration file and command-line overrides. We need a configuration format and parser.

## Decision Drivers
- Zero non-essential third-party dependencies (keeping CMake FetchContent lean and fast).
- Simplicity and transparent readability of benchmark input files.
- Ease of scripting and automated parameter sweeps (e.g. via Python or shell scripts).

## Considered Options
1. Dependency-free custom Key=Value parser (INI-like syntax with `#` comments).
2. Third-party TOML library (`toml++` via FetchContent).
3. JSON parser (`nlohmann/json`).

## Decision Outcome
Chosen option: **Dependency-free custom Key=Value parser**, because:
- The configuration requirements are straightforward: integer grid dimensions, floating-point physical constants ($\alpha, \Delta t$), string boundary conditions, and boolean flags.
- Avoids download overhead, extra compile time, and potential version drift from external library fetches during offline or isolated HPC runs.
- Easy to inspect and format directly in automated benchmark harnesses.

## Consequences
- Positive: No external download dependency needed for the core solver.
- Positive: Compile time for `config.cpp` is instantaneous.
- Negative: Does not support nested tables or lists natively, but none are required for the heat solver parameters.
