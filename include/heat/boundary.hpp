#pragma once

#include <heat/grid.hpp>

#include <string>

namespace heat {

enum class BoundaryType { Dirichlet, Neumann };

enum class InitialConditionType { Sine, Gaussian, Step };

BoundaryType parseBoundaryType(const std::string& str);
InitialConditionType parseInitialConditionType(const std::string& str);

/**
 * @brief Initializes 2D grid according to physical coordinates in [0, 1]^2.
 */
void initializeGrid2D(Grid2D& grid, InitialConditionType ic, double alpha);

/**
 * @brief Initializes 3D grid according to physical coordinates in [0, 1]^3.
 */
void initializeGrid3D(Grid3D& grid, InitialConditionType ic, double alpha);

/**
 * @brief Applies boundary conditions to 2D grid halo cells.
 */
void applyBoundary2D(Grid2D& grid, BoundaryType bc, double t, double alpha);

/**
 * @brief Applies boundary conditions to 3D grid halo cells.
 */
void applyBoundary3D(Grid3D& grid, BoundaryType bc, double t, double alpha);

/**
 * @brief Computes total energy (integral of u over domain) in 2D.
 */
[[nodiscard]] double computeTotalEnergy2D(const Grid2D& grid, double dx, double dy);

/**
 * @brief Computes total energy in 3D.
 */
[[nodiscard]] double computeTotalEnergy3D(const Grid3D& grid, double dx, double dy, double dz);

/**
 * @brief Exact analytic sine-mode decay solution for 1D, 2D, and 3D.
 * u(x, y, z, t) = sin(kx * pi * x) * sin(ky * pi * y) * sin(kz * pi * z) * exp(-k_sq * pi^2 * alpha
 * * t)
 */
[[nodiscard]] double analyticSine1D(double x, double t, double alpha, double kx = 1.0);
[[nodiscard]] double analyticSine2D(double x, double y, double t, double alpha, double kx = 1.0,
                                    double ky = 1.0);
[[nodiscard]] double analyticSine3D(double x, double y, double z, double t, double alpha,
                                    double kx = 1.0, double ky = 1.0, double kz = 1.0);

} // namespace heat
