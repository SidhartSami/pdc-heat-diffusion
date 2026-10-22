#include <heat/boundary.hpp>
#include <heat/config.hpp>
#include <heat/solver.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <vector>

// 1. Analytic Sine Mode Verification in 1D, 2D, and 3D
TEST(SerialSolverTest, AnalyticSineDecay1D) {
    const double alpha = 1.0;
    const double t = 0.05;
    const double x = 0.35;
    const double expected = std::sin(3.14159265358979323846 * x) *
                            std::exp(-3.14159265358979323846 * 3.14159265358979323846 * alpha * t);
    EXPECT_NEAR(heat::analyticSine1D(x, t, alpha), expected, 1e-12);
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
    // With 64x64 grid and dt=2e-5, max error should be well under 1e-3
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

// 2. Empirical Spatial Convergence Test (2x refinement drops error by ~4x)
TEST(SerialSolverTest, SpatialConvergenceOrder2D) {
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
        // Keep diffusion number r = alpha * dt / dx^2 constant so spatial truncation error
        // dominates For n=32: dx=1/32, dx^2 ~ 9.765e-4. Let r = 0.2 -> dt = 0.2 * dx^2 / alpha
        const double dx = 1.0 / n;
        cfg.dt = 0.15 * (dx * dx) / alpha;
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
        const double l2_err = std::sqrt(l2_err_sq / (n * n));
        errors.push_back(l2_err);
    }

    const double ratio1 = errors[0] / errors[1];
    const double ratio2 = errors[1] / errors[2];

    std::cout << "[CONVERGENCE] N=32 Err: " << errors[0] << "\n"
              << "[CONVERGENCE] N=64 Err: " << errors[1] << " (Ratio: " << ratio1 << ")\n"
              << "[CONVERGENCE] N=128 Err: " << errors[2] << " (Ratio: " << ratio2 << ")\n";

    // Second-order finite difference spatial accuracy expects ratio ~ 4.0
    EXPECT_GT(ratio1, 3.6);
    EXPECT_LT(ratio1, 4.4);
    EXPECT_GT(ratio2, 3.6);
    EXPECT_LT(ratio2, 4.4);
}

// 3. Neumann Boundary Energy Conservation Test
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

    // Energy must be conserved within machine round-off precision
    EXPECT_NEAR(energy_init, energy_final, 1e-12);
}

// 4. Stability Guard Enforced in Solver
TEST(SerialSolverTest, StabilityGuardThrows) {
    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 64;
    cfg.ny = 64;
    cfg.alpha = 1.0;
    cfg.dt = 0.01; // Blatantly unstable (stability limit ~6.1e-5)
    EXPECT_THROW(heat::SerialSolver solver(cfg), std::runtime_error);
}
