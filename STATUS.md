# Project Status

All gates G0-G6 are implemented. Awaiting final review by the authors.

| Gate | Scope | State | Evidence |
|---|---|---|---|
| G0 | Scaffold, CMake presets, CI, config parser | Done | `CMakePresets.json`, `.github/workflows/ci.yml` |
| G1 | Serial solver, analytic sine decay, O(dx^2) convergence | Done | `tests/test_serial_solver.cpp`, ADR 0003 |
| G2 | MPI Cartesian decomposition, halo exchange, serial==parallel for 1-8 ranks, ASan/LSan | Done | `tests/test_mpi.cpp`, ADR 0004/0005 |
| G3 | OpenMP, first-touch, pinning, metadata, benchmarks | Done | `bench/benchmark_g3_summary.csv`, README |
| G4 | 2D blocking (null, reverted), 3D blocking (-6.3%), default-path check | Done | `docs/g4_optimization_notes.md` |
| G5 | Strong/weak scaling, hybrid comparison, interleaved overlap test | Done | `results/g5/`, `docs/g5_scaling_plan.md` section 5, `analysis/figures/` |
| G6 | Parallel VTI/PVD output, checkpoint/restart, visualization, report, viva prep, demo | Done | `src/io.cpp`, `analysis/visualize.py`, `docs/report/report.md`, `demo.sh` |

## Verification summary
- 23 tests pass in the release build; MPI tests pass at 1, 2, 3, 4, 6, 8 ranks.
- Restart is bit-identical (`Max Diff = 0`) in 2D and 3D at 1-8 ranks; mismatched checkpoints are rejected.
- Headline results: 2.80x (2D) and 3.22x (3D) on 4 P-cores with pure MPI; overlap shows no measurable difference.

- ASan + UBSan + LSan re-run on the final code (including the new I/O): exit 0 and zero reports at 1, 2, 3, 4, 6, 8 ranks (`tests/run_asan_mpi_tests.sh` environment).
- Proposal-size run (8192^2, 5000 steps): 452.6 s on 1 core, 184.8 s on 4 cores (2.45x), single run each.

## Known limitations
- Single machine (i5-12500H under WSL2), scaling to 4 P-cores only; no multi-node runs.
- Restart needs the same rank count and grid.
- Run-to-run spread is several percent in some configurations; see `results/g5/g5_summary.md`.
- clang-tidy was last run at Gate 2 and was not re-run for the G5/G6 additions.
- CI file exists but has not been exercised on a hosted runner from this branch.

## How to demo
`./demo.sh` (in WSL/Linux). Figures: `analysis/figures/`. Report: `docs/report/report.md`.
