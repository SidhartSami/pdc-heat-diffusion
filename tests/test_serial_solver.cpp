#include <heat/boundary.hpp>
#include <heat/config.hpp>
#include <heat/grid.hpp>
#include <heat/solver.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

// 1. Analytic Sine Mode Verification in 1D, 2D, and 3D
TEST(SerialSolverTest, AnalyticSineDecay1D) {
    // 1D test verifies explicit 1D grid computation & decay formula
    const int nx = 64;
    const double dx = 1.0 / nx;
    const double alpha = 1.0;
    const double dt = 0.00005; // 1D stability limit: dx^2 / (2 * alpha) = 1 / (2 * 4096) ~ 1.22e-4
    const int steps = 100;
    const double t_final = dt * steps;

    heat::Grid1D u_old(nx);
    heat::Grid1D u_new(nx);

    // Initial condition: sin(pi * x)
    for (int i = 1; i <= nx; ++i) {
        const double x = (i - 0.5) * dx;
        u_old(i) = std::sin(3.14159265358979323846 * x);
    }

    const double r = alpha * dt / (dx * dx);
    for (int step = 0; step < steps; ++step) {
        // Dirichlet boundary: u(0) = u(1) = 0
        u_old(0) = -u_old(1);
        u_old(nx + 1) = -u_old(nx);

        for (int i = 1; i <= nx; ++i) {
            u_new(i) = u_old(i) + r * (u_old(i + 1) - 2.0 * u_old(i) + u_old(i - 1));
        }
        u_old.swap(u_new);
    }

    double max_err = 0.0;
    for (int i = 1; i <= nx; ++i) {
        const double x = (i - 0.5) * dx;
        const double exact = heat::analyticSine1D(x, t_final, alpha);
        max_err = std::max(max_err, std::abs(u_old(i) - exact));
    }
    std::cout << "[1D SINE DECAY] Max Pointwise Error: " << max_err << "\n";
    EXPECT_LT(max_err, 1e-3);
}

TEST(SerialSolverTest, AnalyticSineDecay2D) {
    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 64;
    cfg.ny = 64;
    cfg.alpha = 1.0;
    cfg.dt = 0.00002;
    cfg.steps = 50;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";

    heat::SerialSolver solver(cfg);
    heat::Grid2D grid = solver.run2D();

    const double t_final = cfg.dt * cfg.steps;
    const double dx = 1.0 / cfg.nx;
    const double dy = 1.0 / cfg.ny;

    double max_err = 0.0;
    for (int j = 1; j <= cfg.ny; ++j) {
        const double y = (j - 0.5) * dy;
        for (int i = 1; i <= cfg.nx; ++i) {
            const double x = (i - 0.5) * dx;
            const double exact = heat::analyticSine2D(x, y, t_final, cfg.alpha);
            max_err = std::max(max_err, std::abs(grid(i, j) - exact));
        }
    }
    EXPECT_LT(max_err, 1e-3);
}

TEST(SerialSolverTest, AnalyticSineDecay3D) {
    heat::Config cfg;
    cfg.dimension = 3;
    cfg.nx = 32;
    cfg.ny = 32;
    cfg.nz = 32;
    cfg.alpha = 1.0;
    cfg.dt = 0.00001;
    cfg.steps = 30;
    cfg.boundary = "dirichlet";
    cfg.initial_condition = "sine";

    heat::SerialSolver solver(cfg);
    heat::Grid3D grid = solver.run3D();

    const double t_final = cfg.dt * cfg.steps;
    const double dx = 1.0 / cfg.nx;
    const double dy = 1.0 / cfg.ny;
    const double dz = 1.0 / cfg.nz;

    double max_err = 0.0;
    for (int k = 1; k <= cfg.nz; ++k) {
        const double z = (k - 0.5) * dz;
        for (int j = 1; j <= cfg.ny; ++j) {
            const double y = (j - 0.5) * dy;
            for (int i = 1; i <= cfg.nx; ++i) {
                const double x = (i - 0.5) * dx;
                const double exact = heat::analyticSine3D(x, y, z, t_final, cfg.alpha);
                max_err = std::max(max_err, std::abs(grid(i, j, k) - exact));
            }
        }
    }
    EXPECT_LT(max_err, 2e-3);
}

// 2. Full-Scheme Convergence (2D) at Fixed Courant Number r = 0.15
TEST(SerialSolverTest, ConvergenceOrder2D) {
    const std::vector<int> resolutions = {32, 64, 128};
    std::vector<double> errors;
    const double t_target = 0.001;
    const double alpha = 1.0;

    for (int n : resolutions) {
        heat::Config cfg;
        cfg.dimension = 2;
        cfg.nx = n;
        cfg.ny = n;
        cfg.alpha = alpha;
        const double dx = 1.0 / n;
        cfg.dt = 0.15 * (dx * dx) / alpha; // Fixed Courant r = 0.15
        cfg.steps = static_cast<int>(std::round(t_target / cfg.dt));
        cfg.boundary = "dirichlet";
        cfg.initial_condition = "sine";

        heat::SerialSolver solver(cfg);
        heat::Grid2D grid = solver.run2D();

        const double t_final = cfg.dt * cfg.steps;
        double l2_err_sq = 0.0;
        for (int j = 1; j <= n; ++j) {
            const double y = (j - 0.5) * dx;
            for (int i = 1; i <= n; ++i) {
                const double x = (i - 0.5) * dx;
                const double exact = heat::analyticSine2D(x, y, t_final, alpha);
                const double diff = grid(i, j) - exact;
                l2_err_sq += diff * diff;
            }
        }
        // Discrete mean-square L2 norm over interior physical cells
        const double l2_err = std::sqrt(l2_err_sq / (n * n));
        errors.push_back(l2_err);
    }

    const double ratio1 = errors[0] / errors[1];
    const double ratio2 = errors[1] / errors[2];

    std::cout << "[CONVERGENCE 2D] N=32  L2 Err: " << std::scientific << errors[0] << "\n"
              << "[CONVERGENCE 2D] N=64  L2 Err: " << errors[1] << " (Ratio: " << std::fixed
              << std::setprecision(4) << ratio1 << ")\n"
              << "[CONVERGENCE 2D] N=128 L2 Err: " << std::scientific << errors[2]
              << " (Ratio: " << std::fixed << std::setprecision(4) << ratio2 << ")\n";

    EXPECT_GT(ratio1, 3.6);
    EXPECT_LT(ratio1, 4.4);
    EXPECT_GT(ratio2, 3.6);
    EXPECT_LT(ratio2, 4.4);
}

// 3. Full-Scheme Convergence (3D) at Fixed Courant Number r = 0.10
TEST(SerialSolverTest, ConvergenceOrder3D) {
    // We use N = 24, 48, 96 to reach the asymptotic O(dx^2) regime
    const std::vector<int> resolutions = {24, 48, 96};
    std::vector<double> errors;
    const double t_target = 0.001;
    const double alpha = 1.0;

    for (int n : resolutions) {
        heat::Config cfg;
        cfg.dimension = 3;
        cfg.nx = n;
        cfg.ny = n;
        cfg.nz = n;
        cfg.alpha = alpha;
        const double dx = 1.0 / n;
        cfg.dt = 0.10 * (dx * dx) / alpha; // Fixed Courant r = 0.10 (stable for 3D <= 1/6)
        cfg.steps = static_cast<int>(std::round(t_target / cfg.dt));
        cfg.boundary = "dirichlet";
        cfg.initial_condition = "sine";

        heat::SerialSolver solver(cfg);
        heat::Grid3D grid = solver.run3D();

        const double t_final = cfg.dt * cfg.steps;
        double l2_err_sq = 0.0;
        for (int k = 1; k <= n; ++k) {
            const double z = (k - 0.5) * dx;
            for (int j = 1; j <= n; ++j) {
                const double y = (j - 0.5) * dx;
                for (int i = 1; i <= n; ++i) {
                    const double x = (i - 0.5) * dx;
                    const double exact = heat::analyticSine3D(x, y, z, t_final, alpha);
                    const double diff = grid(i, j, k) - exact;
                    l2_err_sq += diff * diff;
                }
            }
        }
        const double l2_err = std::sqrt(l2_err_sq / (static_cast<double>(n) * n * n));
        errors.push_back(l2_err);
    }

    const double ratio1 = errors[0] / errors[1];
    const double ratio2 = errors[1] / errors[2];

    std::cout << "[CONVERGENCE 3D] N=24 L2 Err: " << std::scientific << errors[0] << "\n"
              << "[CONVERGENCE 3D] N=48 L2 Err: " << errors[1] << " (Ratio: " << std::fixed
              << std::setprecision(4) << ratio1 << ")\n"
              << "[CONVERGENCE 3D] N=96 L2 Err: " << std::scientific << errors[2]
              << " (Ratio: " << std::fixed << std::setprecision(4) << ratio2 << ")\n";

    EXPECT_GT(ratio1, 3.6);
    EXPECT_LT(ratio1, 4.4);
    EXPECT_GT(ratio2, 3.6);
    EXPECT_LT(ratio2, 4.4);
}

// 4. Neumann Boundary Energy Conservation Test with exact numbers logged
TEST(SerialSolverTest, NeumannEnergyConservation2D) {
    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 48;
    cfg.ny = 48;
    cfg.alpha = 0.8;
    cfg.dt = 0.00005;
    cfg.steps = 100;
    cfg.boundary = "neumann";
    cfg.initial_condition = "gaussian";

    heat::SerialSolver solver(cfg);

    heat::Grid2D initial_grid(cfg.nx, cfg.ny);
    heat::initializeGrid2D(initial_grid, heat::InitialConditionType::Gaussian, cfg.alpha);
    const double dx = 1.0 / cfg.nx;
    const double dy = 1.0 / cfg.ny;
    const double energy_init = heat::computeTotalEnergy2D(initial_grid, dx, dy);

    heat::Grid2D final_grid = solver.run2D();
    const double energy_final = heat::computeTotalEnergy2D(final_grid, dx, dy);
    const double diff = std::abs(energy_final - energy_init);

    std::cout << "[NEUMANN ENERGY 2D] Grid: " << cfg.nx << "x" << cfg.ny << "\n"
              << "[NEUMANN ENERGY 2D] Initial Energy: " << std::setprecision(16) << energy_init
              << "\n"
              << "[NEUMANN ENERGY 2D] Final Energy:   " << std::setprecision(16) << energy_final
              << "\n"
              << "[NEUMANN ENERGY 2D] Difference:     " << std::scientific << diff << "\n";

    // Gaussian exp(-(rx^2 + ry^2) / 0.02) centered at (0.5, 0.5):
    // 2D integral: int exp(-x^2 / (2*sigma^2)) dx * int exp(-y^2 / (2*sigma^2)) dy
    // where 2*sigma^2 = 0.02 -> sigma = 0.1
    // (sqrt(2*pi)*sigma)^2 = 2*pi*sigma^2 = 2 * pi * 0.01 = 0.02 * pi = 2*pi / 100 ~
    // 0.06283185307...
    EXPECT_NEAR(energy_init, 2.0 * 3.14159265358979323846 / 100.0, 1e-4);
    EXPECT_LT(diff, 1e-12);
}

// 5. Stability Guard Enforced in Solver
TEST(SerialSolverTest, StabilityGuardThrows) {
    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 64;
    cfg.ny = 64;
    cfg.alpha = 1.0;
    cfg.dt = 0.01;
    EXPECT_THROW(heat::SerialSolver solver(cfg), std::runtime_error);
}
