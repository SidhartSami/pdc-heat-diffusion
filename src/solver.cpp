#include <heat/solver.hpp>

#include <iostream>
#include <stdexcept>
#include <utility>

namespace heat {

SerialSolver::SerialSolver(Config config)
    : m_config(std::move(config)) {
    if (!m_config.isStable()) {
        throw std::runtime_error(
            "Explicit FTCS stability violation: dt=" + std::to_string(m_config.dt) +
            " > max_stable_dt=" + std::to_string(m_config.maxStableDt()));
    }
}

void SerialSolver::step2D(const Grid2D& u_old, Grid2D& u_new) const {
    const int nx = u_old.nx();
    const int ny = u_old.ny();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);

    for (int j = 1; j <= ny; ++j) {
        for (int i = 1; i <= nx; ++i) {
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
    }
}

void SerialSolver::step3D(const Grid3D& u_old, Grid3D& u_new) const {
    const int nx = u_old.nx();
    const int ny = u_old.ny();
    const int nz = u_old.nz();
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);
    const double dz = 1.0 / static_cast<double>(nz);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);
    const double rz = m_config.alpha * m_config.dt / (dz * dz);

    for (int k = 1; k <= nz; ++k) {
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                const double center = u_old(i, j, k);
                const double laplacian =
                    rx * (u_old(i + 1, j, k) - 2.0 * center + u_old(i - 1, j, k)) +
                    ry * (u_old(i, j + 1, k) - 2.0 * center + u_old(i, j - 1, k)) +
                    rz * (u_old(i, j, k + 1) - 2.0 * center + u_old(i, j, k - 1));
                u_new(i, j, k) = center + laplacian;
            }
        }
    }
}

Grid2D SerialSolver::run2D() const {
    Grid2D u_old(m_config.nx, m_config.ny);
    Grid2D u_new(m_config.nx, m_config.ny);

    const auto ic = parseInitialConditionType(m_config.initial_condition);
    const auto bc = parseBoundaryType(m_config.boundary);

    initializeGrid2D(u_old, ic, m_config.alpha);

    double t = 0.0;
    for (int step = 0; step < m_config.steps; ++step) {
        applyBoundary2D(u_old, bc, t, m_config.alpha);
        step2D(u_old, u_new);
        t += m_config.dt;
        u_old.swap(u_new);
    }
    // Final boundary apply on u_old
    applyBoundary2D(u_old, bc, t, m_config.alpha);
    return u_old;
}

Grid3D SerialSolver::run3D() const {
    Grid3D u_old(m_config.nx, m_config.ny, m_config.nz);
    Grid3D u_new(m_config.nx, m_config.ny, m_config.nz);

    const auto ic = parseInitialConditionType(m_config.initial_condition);
    const auto bc = parseBoundaryType(m_config.boundary);

    initializeGrid3D(u_old, ic, m_config.alpha);

    double t = 0.0;
    for (int step = 0; step < m_config.steps; ++step) {
        applyBoundary3D(u_old, bc, t, m_config.alpha);
        step3D(u_old, u_new);
        t += m_config.dt;
        u_old.swap(u_new);
    }
    applyBoundary3D(u_old, bc, t, m_config.alpha);
    return u_old;
}

} // namespace heat
