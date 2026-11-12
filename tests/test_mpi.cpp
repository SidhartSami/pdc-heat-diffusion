#include <heat/config.hpp>
#include <heat/decomposition.hpp>
#include <heat/grid.hpp>
#include <heat/halo.hpp>
#include <heat/mpi_solver.hpp>
#include <heat/solver.hpp>

#include <gtest/gtest.h>
#include <omp.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

// 1. Ghost Cell Encoding Verification Test:
// Fills each rank's interior cells with a unique value encoding global coordinates:
// val(i, j) = (global_x * 10000.0) + global_y
// After halo exchange, every ghost cell must match the neighbor's interior cell exactly.
TEST(MpiHaloTest, GhostCellsMatchNeighborEncoding2D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int global_nx = 47; // Uneven grid size
    const int global_ny = 31;

    heat::Decomposition2D decomp(global_nx, global_ny, MPI_COMM_WORLD);
    heat::HaloExchange2D halo(decomp);

    heat::Grid2D grid(decomp.localNx(), decomp.localNy());

    // Encode coordinates into interior
    for (int j = 1; j <= decomp.localNy(); ++j) {
        const int gy = decomp.startY() + j;
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const int gx = decomp.startX() + i;
            grid(i, j) = static_cast<double>(gx * 10000 + gy);
        }
    }

    // Exchange halos across Cartesian topology
    halo.exchange(grid);

    int verified_faces = 0;
    // Verify ghost cells against expected global coordinate encodings
    if (decomp.neighborLeft() != MPI_PROC_NULL) {
        verified_faces++;
        const int gx = decomp.startX(); // Left neighbor's column
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(0, j), expected);
        }
    }
    if (decomp.neighborRight() != MPI_PROC_NULL) {
        verified_faces++;
        const int gx = decomp.startX() + decomp.localNx() + 1; // Right neighbor's column
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(decomp.localNx() + 1, j), expected);
        }
    }
    if (decomp.neighborBottom() != MPI_PROC_NULL) {
        verified_faces++;
        const int gy = decomp.startY(); // Bottom neighbor's row
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const int gx = decomp.startX() + i;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(i, 0), expected);
        }
    }
    if (decomp.neighborTop() != MPI_PROC_NULL) {
        verified_faces++;
        const int gy = decomp.startY() + decomp.localNy() + 1; // Top neighbor's row
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const int gx = decomp.startX() + i;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(i, decomp.localNy() + 1), expected);
        }
    }

    if (size > 1) {
        EXPECT_GT(verified_faces, 0);
    }
}

// 2. 3D Ghost Cell Encoding Verification Test
TEST(MpiHaloTest, GhostCellsMatchNeighborEncoding3D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int global_nx = 29;
    const int global_ny = 23;
    const int global_nz = 19;

    heat::Decomposition3D decomp(global_nx, global_ny, global_nz, MPI_COMM_WORLD);
    heat::HaloExchange3D halo(decomp);

    heat::Grid3D grid(decomp.localNx(), decomp.localNy(), decomp.localNz());

    // Encode 3D coordinates: val = gx * 1000000 + gy * 1000 + gz
    for (int k = 1; k <= decomp.localNz(); ++k) {
        const int gz = decomp.startZ() + k;
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const int gx = decomp.startX() + i;
                grid(i, j, k) = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
            }
        }
    }

    halo.exchange(grid);

    int verified_faces = 0;
    if (decomp.neighborLeft() != MPI_PROC_NULL) {
        verified_faces++;
        const int gx = decomp.startX();
        for (int k = 1; k <= decomp.localNz(); ++k) {
            const int gz = decomp.startZ() + k;
            for (int j = 1; j <= decomp.localNy(); ++j) {
                const int gy = decomp.startY() + j;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(0, j, k), expected);
            }
        }
    }
    if (decomp.neighborRight() != MPI_PROC_NULL) {
        verified_faces++;
        const int gx = decomp.startX() + decomp.localNx() + 1;
        for (int k = 1; k <= decomp.localNz(); ++k) {
            const int gz = decomp.startZ() + k;
            for (int j = 1; j <= decomp.localNy(); ++j) {
                const int gy = decomp.startY() + j;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(decomp.localNx() + 1, j, k), expected);
            }
        }
    }
    if (decomp.neighborBottom() != MPI_PROC_NULL) {
        verified_faces++;
        const int gy = decomp.startY();
        for (int k = 1; k <= decomp.localNz(); ++k) {
            const int gz = decomp.startZ() + k;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const int gx = decomp.startX() + i;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(i, 0, k), expected);
            }
        }
    }
    if (decomp.neighborTop() != MPI_PROC_NULL) {
        verified_faces++;
        const int gy = decomp.startY() + decomp.localNy() + 1;
        for (int k = 1; k <= decomp.localNz(); ++k) {
            const int gz = decomp.startZ() + k;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const int gx = decomp.startX() + i;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(i, decomp.localNy() + 1, k), expected);
            }
        }
    }
    if (decomp.neighborBack() != MPI_PROC_NULL) {
        verified_faces++;
        const int gz = decomp.startZ();
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const int gx = decomp.startX() + i;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(i, j, 0), expected);
            }
        }
    }
    if (decomp.neighborFront() != MPI_PROC_NULL) {
        verified_faces++;
        const int gz = decomp.startZ() + decomp.localNz() + 1;
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const int gx = decomp.startX() + i;
                const double expected = static_cast<double>(gx * 1000000 + gy * 1000 + gz);
                EXPECT_DOUBLE_EQ(grid(i, j, decomp.localNz() + 1), expected);
            }
        }
    }

    if (size > 1) {
        EXPECT_GT(verified_faces, 0);
    }
}

// 2. Parallel vs Serial Numerical Equivalence on Uneven 2D Grids
TEST(MpiSolverTest, ParallelEqualsSerial2DUneven) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 67; // Prime / uneven dimensions
    cfg.ny = 53;
    cfg.alpha = 1.0;
    cfg.dt = 0.00002;
    cfg.steps = 40;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";
    cfg.overlap = false; // Baseline synchronous halo exchange

    // 1. Run parallel distributed solver
    heat::MpiSolver mpi_solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition2D decomp(cfg.nx, cfg.ny, MPI_COMM_WORLD);
    heat::Grid2D local_grid = mpi_solver.run2D();
    heat::Grid2D parallel_gathered = mpi_solver.gather2D(local_grid, decomp);

    if (rank == 0) {
        std::cout << "[HYBRID THREAD VERIFICATION] Max Configured Threads: " << omp_get_max_threads()
                  << ", Active Stencil Loop Threads: " << mpi_solver.activeThreadsLastRun() << "\n";
        EXPECT_GT(mpi_solver.activeThreadsLastRun(), 0);
        EXPECT_EQ(mpi_solver.activeThreadsLastRun(), omp_get_max_threads());

        // 2. Run serial reference solver
        heat::SerialSolver serial_solver(cfg);
        heat::Grid2D serial_grid = serial_solver.run2D();

        double max_diff = 0.0;
        for (int j = 1; j <= cfg.ny; ++j) {
            for (int i = 1; i <= cfg.nx; ++i) {
                const double diff = std::abs(parallel_gathered(i, j) - serial_grid(i, j));
                max_diff = std::max(max_diff, diff);
            }
        }
        std::cout << "[PARALLEL EQUALS SERIAL 2D] Ranks: " << size << ", Grid: " << cfg.nx << "x"
                  << cfg.ny << ", Max Diff: " << max_diff << "\n";
        EXPECT_LT(max_diff, 1e-12);
    }
}

// 3. Compute-Communication Overlap Equivalence Test (Overlap ON vs Serial)
TEST(MpiSolverTest, OverlapEqualsSerial2D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 64;
    cfg.ny = 64;
    cfg.alpha = 1.0;
    cfg.dt = 0.00002;
    cfg.steps = 50;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";
    cfg.overlap = true; // Overlap ENABLED

    heat::MpiSolver mpi_solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition2D decomp(cfg.nx, cfg.ny, MPI_COMM_WORLD);
    heat::Grid2D local_grid = mpi_solver.run2D();
    heat::Grid2D parallel_gathered = mpi_solver.gather2D(local_grid, decomp);

    if (rank == 0) {
        heat::SerialSolver serial_solver(cfg);
        heat::Grid2D serial_grid = serial_solver.run2D();

        double max_diff = 0.0;
        for (int j = 1; j <= cfg.ny; ++j) {
            for (int i = 1; i <= cfg.nx; ++i) {
                const double diff = std::abs(parallel_gathered(i, j) - serial_grid(i, j));
                max_diff = std::max(max_diff, diff);
            }
        }
        std::cout << "[OVERLAP EQUALS SERIAL 2D] Ranks: " << size << ", Grid: " << cfg.nx << "x"
                  << cfg.ny << ", Max Diff: " << max_diff << "\n";
        EXPECT_LT(max_diff, 1e-12);
    }
}

// 4. Parallel vs Serial 3D Grid Equivalence
TEST(MpiSolverTest, ParallelEqualsSerial3DUneven) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg;
    cfg.dimension = 3;
    cfg.nx = 37; // Uneven 3D grid
    cfg.ny = 29;
    cfg.nz = 31;
    cfg.alpha = 1.0;
    cfg.dt = 0.00001;
    cfg.steps = 25;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";
    cfg.overlap = true;

    heat::MpiSolver mpi_solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition3D decomp(cfg.nx, cfg.ny, cfg.nz, MPI_COMM_WORLD);
    heat::Grid3D local_grid = mpi_solver.run3D();
    heat::Grid3D parallel_gathered = mpi_solver.gather3D(local_grid, decomp);

    if (rank == 0) {
        heat::SerialSolver serial_solver(cfg);
        heat::Grid3D serial_grid = serial_solver.run3D();

        double max_diff = 0.0;
        for (int k = 1; k <= cfg.nz; ++k) {
            for (int j = 1; j <= cfg.ny; ++j) {
                for (int i = 1; i <= cfg.nx; ++i) {
                    const double diff = std::abs(parallel_gathered(i, j, k) - serial_grid(i, j, k));
                    max_diff = std::max(max_diff, diff);
                }
            }
        }
        std::cout << "[PARALLEL EQUALS SERIAL 3D] Ranks: " << size << ", Grid: " << cfg.nx << "x"
                  << cfg.ny << "x" << cfg.nz << ", Max Diff: " << max_diff << "\n";
        EXPECT_LT(max_diff, 1e-12);
    }
}

// 5. Parallel vs Serial 3D Grid Equivalence with Overlap ON
TEST(MpiSolverTest, OverlapEqualsSerial3D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg;
    cfg.dimension = 3;
    cfg.nx = 32;
    cfg.ny = 32;
    cfg.nz = 32;
    cfg.alpha = 1.0;
    cfg.dt = 0.00001;
    cfg.steps = 20;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";
    cfg.overlap = true;

    heat::MpiSolver mpi_solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition3D decomp(cfg.nx, cfg.ny, cfg.nz, MPI_COMM_WORLD);
    heat::Grid3D local_grid = mpi_solver.run3D();
    heat::Grid3D parallel_gathered = mpi_solver.gather3D(local_grid, decomp);

    if (rank == 0) {
        heat::SerialSolver serial_solver(cfg);
        heat::Grid3D serial_grid = serial_solver.run3D();

        double max_diff = 0.0;
        for (int k = 1; k <= cfg.nz; ++k) {
            for (int j = 1; j <= cfg.ny; ++j) {
                for (int i = 1; i <= cfg.nx; ++i) {
                    const double diff = std::abs(parallel_gathered(i, j, k) - serial_grid(i, j, k));
                    max_diff = std::max(max_diff, diff);
                }
            }
        }
        std::cout << "[OVERLAP EQUALS SERIAL 3D] Ranks: " << size << ", Grid: " << cfg.nx << "x"
                  << cfg.ny << "x" << cfg.nz << ", Max Diff: " << max_diff << "\n";
        EXPECT_LT(max_diff, 1e-12);
    }
}

// 6. Neumann Boundary Condition Verification under MPI
TEST(MpiSolverTest, NeumannEnergyConservationMPI2D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 48;
    cfg.ny = 48;
    cfg.alpha = 0.8;
    cfg.dt = 0.00005;
    cfg.steps = 50;
    cfg.boundary = "neumann";
    cfg.initial_condition = "gaussian";
    cfg.overlap = false;

    heat::MpiSolver mpi_solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition2D decomp(cfg.nx, cfg.ny, MPI_COMM_WORLD);
    heat::Grid2D local_grid = mpi_solver.run2D();
    heat::Grid2D parallel_gathered = mpi_solver.gather2D(local_grid, decomp);

    if (rank == 0) {
        const double dx = 1.0 / cfg.nx;
        const double dy = 1.0 / cfg.ny;
        const double energy_final = heat::computeTotalEnergy2D(parallel_gathered, dx, dy);

        // Compare against serial Neumann solver
        heat::SerialSolver serial_solver(cfg);
        heat::Grid2D serial_grid = serial_solver.run2D();
        const double energy_serial = heat::computeTotalEnergy2D(serial_grid, dx, dy);

        std::cout << "[NEUMANN MPI EQUIVALENCE 2D] Ranks: " << size
                  << ", MPI Energy: " << energy_final << ", Serial Energy: " << energy_serial
                  << "\n";
        EXPECT_LE(std::abs(energy_final - energy_serial) / energy_serial, 1e-12);
    }
}

// ---------------------------------------------------------------------------------------------
// G6: checkpoint / restart and VTK snapshot tests
// ---------------------------------------------------------------------------------------------

namespace {

heat::Config makeIoConfig(int dim) {
    heat::Config cfg;
    cfg.dimension = dim;
    cfg.nx = (dim == 2) ? 53 : 21;
    cfg.ny = (dim == 2) ? 41 : 19;
    cfg.nz = 17;
    cfg.alpha = 1.0;
    cfg.dt = 0.0;
    cfg.dt_auto = true;
    cfg.dt = 0.9 * cfg.maxStableDt();
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "gaussian";
    cfg.overlap = true;
    return cfg;
}

} // namespace

TEST(MpiIoTest, RestartIsBitIdentical2D) {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const std::string prefix = "output/test_ckpt_2d/ckpt";

    heat::Config full = makeIoConfig(2);
    full.steps = 40;
    heat::MpiSolver full_solver(full, MPI_COMM_WORLD);
    heat::Decomposition2D decomp(full.nx, full.ny, MPI_COMM_WORLD);
    const heat::Grid2D full_global = full_solver.gather2D(full_solver.run2D(), decomp);

    heat::Config first = full;
    first.steps = 20;
    first.checkpoint_freq = 20;
    first.checkpoint_path = prefix;
    heat::MpiSolver first_solver(first, MPI_COMM_WORLD);
    (void)first_solver.run2D();

    heat::Config second = full;
    second.restart_from = prefix;
    heat::MpiSolver second_solver(second, MPI_COMM_WORLD);
    const heat::Grid2D restarted_global = second_solver.gather2D(second_solver.run2D(), decomp);

    if (rank == 0) {
        double max_diff = 0.0;
        for (int j = 1; j <= full.ny; ++j) {
            for (int i = 1; i <= full.nx; ++i) {
                max_diff = std::max(max_diff, std::abs(full_global(i, j) - restarted_global(i, j)));
            }
        }
        std::cout << "[RESTART 2D] Max Diff vs uninterrupted run: " << max_diff << "\n";
        EXPECT_EQ(max_diff, 0.0);
    }
}

TEST(MpiIoTest, RestartIsBitIdentical3D) {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const std::string prefix = "output/test_ckpt_3d/ckpt";

    heat::Config full = makeIoConfig(3);
    full.steps = 24;
    heat::MpiSolver full_solver(full, MPI_COMM_WORLD);
    heat::Decomposition3D decomp(full.nx, full.ny, full.nz, MPI_COMM_WORLD);
    const heat::Grid3D full_global = full_solver.gather3D(full_solver.run3D(), decomp);

    heat::Config first = full;
    first.steps = 10;
    first.checkpoint_freq = 10;
    first.checkpoint_path = prefix;
    heat::MpiSolver first_solver(first, MPI_COMM_WORLD);
    (void)first_solver.run3D();

    heat::Config second = full;
    second.restart_from = prefix;
    heat::MpiSolver second_solver(second, MPI_COMM_WORLD);
    const heat::Grid3D restarted_global = second_solver.gather3D(second_solver.run3D(), decomp);

    if (rank == 0) {
        double max_diff = 0.0;
        for (int k = 1; k <= full.nz; ++k) {
            for (int j = 1; j <= full.ny; ++j) {
                for (int i = 1; i <= full.nx; ++i) {
                    max_diff = std::max(
                        max_diff, std::abs(full_global(i, j, k) - restarted_global(i, j, k)));
                }
            }
        }
        std::cout << "[RESTART 3D] Max Diff vs uninterrupted run: " << max_diff << "\n";
        EXPECT_EQ(max_diff, 0.0);
    }
}

TEST(MpiIoTest, RestartRejectsMismatchedGrid) {
    const std::string prefix = "output/test_ckpt_mismatch/ckpt";
    heat::Config first = makeIoConfig(2);
    first.steps = 5;
    first.checkpoint_freq = 5;
    first.checkpoint_path = prefix;
    heat::MpiSolver first_solver(first, MPI_COMM_WORLD);
    (void)first_solver.run2D();

    heat::Config bad = first;
    bad.checkpoint_freq = 0;
    bad.nx = first.nx + 4; // different grid -> checkpoint must be refused on every rank
    bad.dt = 0.9 * bad.maxStableDt();
    bad.steps = 10;
    bad.restart_from = prefix;
    heat::MpiSolver bad_solver(bad, MPI_COMM_WORLD);
    EXPECT_THROW((void)bad_solver.run2D(), std::runtime_error);
}

TEST(MpiIoTest, VtkSnapshotContentsMatchGrid2D) {
    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config cfg = makeIoConfig(2);
    cfg.steps = 6;
    cfg.snapshot_freq = 3;
    cfg.output_path = "output/test_vtk_2d";
    heat::MpiSolver solver(cfg, MPI_COMM_WORLD);
    heat::Decomposition2D decomp(cfg.nx, cfg.ny, MPI_COMM_WORLD);
    const heat::Grid2D local = solver.run2D();
    const heat::Grid2D global = solver.gather2D(local, decomp);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // 3 snapshots: steps 0, 3, 6; every piece must exist and the .pvd must list all three.
        std::ifstream pvd("output/test_vtk_2d/heat.pvd");
        ASSERT_TRUE(pvd.good());
        const std::string pvd_text((std::istreambuf_iterator<char>(pvd)),
                                   std::istreambuf_iterator<char>());
        EXPECT_NE(pvd_text.find("heat_000000.pvti"), std::string::npos);
        EXPECT_NE(pvd_text.find("heat_000003.pvti"), std::string::npos);
        EXPECT_NE(pvd_text.find("heat_000006.pvti"), std::string::npos);

        // Final snapshot, every rank's piece: payload must equal the gathered global field.
        for (int r = 0; r < size; ++r) {
            std::ifstream piece("output/test_vtk_2d/heat_000006_r" + std::to_string(r) + ".vti",
                                std::ios::binary);
            ASSERT_TRUE(piece.good());
            const std::string text((std::istreambuf_iterator<char>(piece)),
                                   std::istreambuf_iterator<char>());
            const auto extent_pos = text.find("<Piece Extent=\"");
            ASSERT_NE(extent_pos, std::string::npos);
            int x0 = 0, x1 = 0, y0 = 0, y1 = 0, z0 = 0, z1 = 0;
            ASSERT_EQ(std::sscanf(text.c_str() + extent_pos, "<Piece Extent=\"%d %d %d %d %d %d\"",
                                  &x0, &x1, &y0, &y1, &z0, &z1),
                      6);
            const auto data_pos = text.find("_", text.find("<AppendedData")) + 1;
            std::uint32_t nbytes = 0;
            std::memcpy(&nbytes, text.data() + data_pos, sizeof(nbytes));
            const int lnx = x1 - x0;
            const int lny = y1 - y0;
            ASSERT_EQ(nbytes, static_cast<std::uint32_t>(lnx * lny * sizeof(double)));
            const char* payload = text.data() + data_pos + sizeof(nbytes);
            double max_diff = 0.0;
            for (int j = 0; j < lny; ++j) {
                for (int i = 0; i < lnx; ++i) {
                    double v = 0.0;
                    std::memcpy(&v, payload + (static_cast<std::size_t>(j) * lnx + i) * sizeof(double),
                                sizeof(double));
                    max_diff = std::max(max_diff, std::abs(v - global(x0 + i + 1, y0 + j + 1)));
                }
            }
            EXPECT_EQ(max_diff, 0.0) << "piece of rank " << r;
        }
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    int result = RUN_ALL_TESTS();
    MPI_Finalize();
    return result;
}
