#include <heat/boundary.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace heat {

static constexpr double PI = 3.14159265358979323846;

BoundaryType parseBoundaryType(const std::string& str) {
    if (str == "dirichlet") {
        return BoundaryType::Dirichlet;
    }
    if (str == "neumann") {
        return BoundaryType::Neumann;
    }
    throw std::runtime_error("Unknown boundary type: " + str);
}

InitialConditionType parseInitialConditionType(const std::string& str) {
    if (str == "sine") {
        return InitialConditionType::Sine;
    }
    if (str == "gaussian") {
        return InitialConditionType::Gaussian;
    }
    if (str == "step") {
        return InitialConditionType::Step;
    }
    throw std::runtime_error("Unknown initial condition type: " + str);
}

double analyticSine1D(double x, double t, double alpha, double kx) {
    const double decay = std::exp(-kx * kx * PI * PI * alpha * t);
    return std::sin(kx * PI * x) * decay;
}

double analyticSine2D(double x, double y, double t, double alpha, double kx, double ky) {
    const double decay = std::exp(-(kx * kx + ky * ky) * PI * PI * alpha * t);
    return std::sin(kx * PI * x) * std::sin(ky * PI * y) * decay;
}

double analyticSine3D(double x, double y, double z, double t, double alpha, double kx, double ky,
                      double kz) {
    const double decay = std::exp(-(kx * kx + ky * ky + kz * kz) * PI * PI * alpha * t);
    return std::sin(kx * PI * x) * std::sin(ky * PI * y) * std::sin(kz * PI * z) * decay;
}

void initializeGrid2D(Grid2D& grid, InitialConditionType ic, double alpha) {
    const int nx = grid.nx();
    const int ny = grid.ny();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);

    for (int j = 1; j <= ny; ++j) {
        const double y = (static_cast<double>(j) - 0.5) * dy;
        for (int i = 1; i <= nx; ++i) {
            const double x = (static_cast<double>(i) - 0.5) * dx;
            switch (ic) {
            case InitialConditionType::Sine:
                grid(i, j) = analyticSine2D(x, y, 0.0, alpha);
                break;
            case InitialConditionType::Gaussian: {
                const double rx = x - 0.5;
                const double ry = y - 0.5;
                grid(i, j) = std::exp(-(rx * rx + ry * ry) / 0.02);
                break;
            }
            case InitialConditionType::Step:
                grid(i, j) = (x >= 0.25 && x <= 0.75 && y >= 0.25 && y <= 0.75) ? 1.0 : 0.0;
                break;
            }
        }
    }
}

void initializeGrid3D(Grid3D& grid, InitialConditionType ic, double alpha) {
    const int nx = grid.nx();
    const int ny = grid.ny();
    const int nz = grid.nz();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);
    const double dz = 1.0 / static_cast<double>(nz);

    for (int k = 1; k <= nz; ++k) {
        const double z = (static_cast<double>(k) - 0.5) * dz;
        for (int j = 1; j <= ny; ++j) {
            const double y = (static_cast<double>(j) - 0.5) * dy;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(i) - 0.5) * dx;
                switch (ic) {
                case InitialConditionType::Sine:
                    grid(i, j, k) = analyticSine3D(x, y, z, 0.0, alpha);
                    break;
                case InitialConditionType::Gaussian: {
                    const double rx = x - 0.5;
                    const double ry = y - 0.5;
                    const double rz = z - 0.5;
                    grid(i, j, k) = std::exp(-(rx * rx + ry * ry + rz * rz) / 0.02);
                    break;
                }
                case InitialConditionType::Step:
                    grid(i, j, k) =
                        (x >= 0.25 && x <= 0.75 && y >= 0.25 && y <= 0.75 && z >= 0.25 && z <= 0.75)
                            ? 1.0
                            : 0.0;
                    break;
                }
            }
        }
    }
}

void applyBoundary2D(Grid2D& grid, BoundaryType bc, double t, double alpha) {
    const int nx = grid.nx();
    const int ny = grid.ny();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);

    if (bc == BoundaryType::Dirichlet) {
        // Left & Right ghost columns (x = 0 and x = nx + 1)
        for (int j = 1; j <= ny; ++j) {
            const double y = (static_cast<double>(j) - 0.5) * dy;
            // Physical boundaries at x=0 and x=1 are 0 for the sine mode
            grid(0, j) = -grid(1, j) + 2.0 * analyticSine2D(0.0, y, t, alpha);
            grid(nx + 1, j) = -grid(nx, j) + 2.0 * analyticSine2D(1.0, y, t, alpha);
        }
        // Bottom & Top ghost rows (y = 0 and y = ny + 1)
        for (int i = 1; i <= nx; ++i) {
            const double x = (static_cast<double>(i) - 0.5) * dx;
            grid(i, 0) = -grid(i, 1) + 2.0 * analyticSine2D(x, 0.0, t, alpha);
            grid(i, ny + 1) = -grid(i, ny) + 2.0 * analyticSine2D(x, 1.0, t, alpha);
        }
    } else if (bc == BoundaryType::Neumann) {
        // Zero flux: du/dn = 0 -> ghost = nearest interior cell
        for (int j = 1; j <= ny; ++j) {
            grid(0, j) = grid(1, j);
            grid(nx + 1, j) = grid(nx, j);
        }
        for (int i = 1; i <= nx; ++i) {
            grid(i, 0) = grid(i, 1);
            grid(i, ny + 1) = grid(i, ny);
        }
    }
}

void applyBoundary3D(Grid3D& grid, BoundaryType bc, double t, double alpha) {
    const int nx = grid.nx();
    const int ny = grid.ny();
    const int nz = grid.nz();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);
    const double dz = 1.0 / static_cast<double>(nz);

    if (bc == BoundaryType::Dirichlet) {
        // X-faces
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(k) - 0.5) * dz;
            for (int j = 1; j <= ny; ++j) {
                const double y = (static_cast<double>(j) - 0.5) * dy;
                grid(0, j, k) = -grid(1, j, k) + 2.0 * analyticSine3D(0.0, y, z, t, alpha);
                grid(nx + 1, j, k) = -grid(nx, j, k) + 2.0 * analyticSine3D(1.0, y, z, t, alpha);
            }
        }
        // Y-faces
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(k) - 0.5) * dz;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(i) - 0.5) * dx;
                grid(i, 0, k) = -grid(i, 1, k) + 2.0 * analyticSine3D(x, 0.0, z, t, alpha);
                grid(i, ny + 1, k) = -grid(i, ny, k) + 2.0 * analyticSine3D(x, 1.0, z, t, alpha);
            }
        }
        // Z-faces
        for (int j = 1; j <= ny; ++j) {
            const double y = (static_cast<double>(j) - 0.5) * dy;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(i) - 0.5) * dx;
                grid(i, j, 0) = -grid(i, j, 1) + 2.0 * analyticSine3D(x, y, 0.0, t, alpha);
                grid(i, j, nz + 1) = -grid(i, j, nz) + 2.0 * analyticSine3D(x, y, 1.0, t, alpha);
            }
        }
    } else if (bc == BoundaryType::Neumann) {
        for (int k = 1; k <= nz; ++k) {
            for (int j = 1; j <= ny; ++j) {
                grid(0, j, k) = grid(1, j, k);
                grid(nx + 1, j, k) = grid(nx, j, k);
            }
        }
        for (int k = 1; k <= nz; ++k) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, 0, k) = grid(i, 1, k);
                grid(i, ny + 1, k) = grid(i, ny, k);
            }
        }
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, j, 0) = grid(i, j, 1);
                grid(i, j, nz + 1) = grid(i, j, nz);
            }
        }
    }
}

double computeTotalEnergy2D(const Grid2D& grid, double dx, double dy) {
    double total = 0.0;
    const int nx = grid.nx();
    const int ny = grid.ny();
    for (int j = 1; j <= ny; ++j) {
        for (int i = 1; i <= nx; ++i) {
            total += grid(i, j);
        }
    }
    return total * dx * dy;
}

double computeTotalEnergy3D(const Grid3D& grid, double dx, double dy, double dz) {
    double total = 0.0;
    const int nx = grid.nx();
    const int ny = grid.ny();
    const int nz = grid.nz();
    for (int k = 1; k <= nz; ++k) {
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                total += grid(i, j, k);
            }
        }
    }
    return total * dx * dy * dz;
}

} // namespace heat
