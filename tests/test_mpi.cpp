#include <heat/config.hpp>
#include <heat/decomposition.hpp>
#include <heat/grid.hpp>
#include <heat/halo.hpp>
#include <heat/mpi_solver.hpp>
#include <heat/solver.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

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

    // Verify ghost cells against expected global coordinate encodings
    if (decomp.neighborLeft() != MPI_PROC_NULL) {
        const int gx = decomp.startX(); // Left neighbor's column
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(0, j), expected);
        }
    }
    if (decomp.neighborRight() != MPI_PROC_NULL) {
        const int gx = decomp.startX() + decomp.localNx() + 1; // Right neighbor's column
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const int gy = decomp.startY() + j;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(decomp.localNx() + 1, j), expected);
        }
    }
    if (decomp.neighborBottom() != MPI_PROC_NULL) {
        const int gy = decomp.startY(); // Bottom neighbor's row
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const int gx = decomp.startX() + i;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(i, 0), expected);
        }
    }
    if (decomp.neighborTop() != MPI_PROC_NULL) {
        const int gy = decomp.startY() + decomp.localNy() + 1; // Top neighbor's row
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const int gx = decomp.startX() + i;
            const double expected = static_cast<double>(gx * 10000 + gy);
            EXPECT_DOUBLE_EQ(grid(i, decomp.localNy() + 1), expected);
        }
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    int result = RUN_ALL_TESTS();
    MPI_Finalize();
    return result;
}
