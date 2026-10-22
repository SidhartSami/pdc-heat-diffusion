#include <heat/mpi_solver.hpp>

#include <cmath>
#include <iostream>
#include <omp.h>
#include <stdexcept>
#include <utility>

namespace heat {

MpiSolver::MpiSolver(Config config, MPI_Comm comm)
    : m_config(std::move(config))
    , m_comm(comm) {
    if (!m_config.isStable()) {
        throw std::runtime_error(
            "Explicit FTCS stability violation: dt=" + std::to_string(m_config.dt) +
            " > max_stable_dt=" + std::to_string(m_config.maxStableDt()));
    }
}

void MpiSolver::applyPhysicalBoundary2D(Grid2D& grid, const Decomposition2D& decomp,
                                        BoundaryType bc, double t) const {
    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);

    // Left physical boundary
    if (decomp.neighborLeft() == MPI_PROC_NULL) {
        if (bc == BoundaryType::Dirichlet) {
            for (int j = 1; j <= ny; ++j) {
                const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
                grid(0, j) = -grid(1, j) + 2.0 * analyticSine2D(0.0, y, t, m_config.alpha);
            }
        } else {
            for (int j = 1; j <= ny; ++j) {
                grid(0, j) = grid(1, j);
            }
        }
    }
    // Right physical boundary
    if (decomp.neighborRight() == MPI_PROC_NULL) {
        if (bc == BoundaryType::Dirichlet) {
            for (int j = 1; j <= ny; ++j) {
                const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
                grid(nx + 1, j) = -grid(nx, j) + 2.0 * analyticSine2D(1.0, y, t, m_config.alpha);
            }
        } else {
            for (int j = 1; j <= ny; ++j) {
                grid(nx + 1, j) = grid(nx, j);
            }
        }
    }
    // Bottom physical boundary
    if (decomp.neighborBottom() == MPI_PROC_NULL) {
        if (bc == BoundaryType::Dirichlet) {
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, 0) = -grid(i, 1) + 2.0 * analyticSine2D(x, 0.0, t, m_config.alpha);
            }
        } else {
            for (int i = 1; i <= nx; ++i) {
                grid(i, 0) = grid(i, 1);
            }
        }
    }
    // Top physical boundary
    if (decomp.neighborTop() == MPI_PROC_NULL) {
        if (bc == BoundaryType::Dirichlet) {
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, ny + 1) = -grid(i, ny) + 2.0 * analyticSine2D(x, 1.0, t, m_config.alpha);
            }
        } else {
            for (int i = 1; i <= nx; ++i) {
                grid(i, ny + 1) = grid(i, ny);
            }
        }
    }
}

void MpiSolver::applyPhysicalBoundary3D(Grid3D& grid, const Decomposition3D& decomp,
                                        BoundaryType bc, double t) const {
    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const int nz = decomp.localNz();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double dz = 1.0 / static_cast<double>(m_config.nz);

    // Left (x = 0)
    if (decomp.neighborLeft() == MPI_PROC_NULL) {
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(decomp.startZ() + k) - 0.5) * dz;
            for (int j = 1; j <= ny; ++j) {
                const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
                grid(0, j, k) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(1, j, k) + 2.0 * analyticSine3D(0.0, y, z, t, m_config.alpha))
                        : grid(1, j, k);
            }
        }
    }
    // Right (x = 1)
    if (decomp.neighborRight() == MPI_PROC_NULL) {
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(decomp.startZ() + k) - 0.5) * dz;
            for (int j = 1; j <= ny; ++j) {
                const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
                grid(nx + 1, j, k) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(nx, j, k) + 2.0 * analyticSine3D(1.0, y, z, t, m_config.alpha))
                        : grid(nx, j, k);
            }
        }
    }
    // Bottom (y = 0)
    if (decomp.neighborBottom() == MPI_PROC_NULL) {
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(decomp.startZ() + k) - 0.5) * dz;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, 0, k) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(i, 1, k) + 2.0 * analyticSine3D(x, 0.0, z, t, m_config.alpha))
                        : grid(i, 1, k);
            }
        }
    }
    // Top (y = 1)
    if (decomp.neighborTop() == MPI_PROC_NULL) {
        for (int k = 1; k <= nz; ++k) {
            const double z = (static_cast<double>(decomp.startZ() + k) - 0.5) * dz;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, ny + 1, k) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(i, ny, k) + 2.0 * analyticSine3D(x, 1.0, z, t, m_config.alpha))
                        : grid(i, ny, k);
            }
        }
    }
    // Back (z = 0)
    if (decomp.neighborBack() == MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, j, 0) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(i, j, 1) + 2.0 * analyticSine3D(x, y, 0.0, t, m_config.alpha))
                        : grid(i, j, 1);
            }
        }
    }
    // Front (z = 1)
    if (decomp.neighborFront() == MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
            for (int i = 1; i <= nx; ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                grid(i, j, nz + 1) =
                    (bc == BoundaryType::Dirichlet)
                        ? (-grid(i, j, nz) + 2.0 * analyticSine3D(x, y, 1.0, t, m_config.alpha))
                        : grid(i, j, nz);
            }
        }
    }
}

void MpiSolver::step2DNoOverlap(Grid2D& u_old, Grid2D& u_new, HaloExchange2D& halo,
                                const Decomposition2D& decomp, BoundaryType bc, double t) const {
    halo.exchange(u_old);
    applyPhysicalBoundary2D(u_old, decomp, bc, t);

    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);

    #pragma omp parallel default(none) \
        shared(u_old, u_new, nx, ny, rx, ry, m_last_active_threads)
    {
        #pragma omp single
        m_last_active_threads = omp_get_num_threads();

        #pragma omp for collapse(2) schedule(static)
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                const double center = u_old(i, j);
                const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                         ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
                u_new(i, j) = center + laplacian;
            }
        }
    }
}

void MpiSolver::step2DOverlap(Grid2D& u_old, Grid2D& u_new, HaloExchange2D& halo,
                              const Decomposition2D& decomp, BoundaryType bc, double t) const {
    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);

    // 1. Post halo exchanges
    halo.startExchange(u_old);

    // 2. Compute interior domain (cells [2, nx-1] x [2, ny-1])
    #pragma omp parallel for collapse(2) default(none) \
        shared(u_old, u_new, nx, ny, rx, ry) schedule(static)
    for (int j = 2; j <= ny - 1; ++j) {
        for (int i = 2; i <= nx - 1; ++i) {
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
    }

    // 3. Complete halo exchange & apply physical boundaries
    halo.finishExchange(u_old);
    applyPhysicalBoundary2D(u_old, decomp, bc, t);

    // 4. Compute boundary shells (j = 1 and j = ny for all i, i = 1 and i = nx for all j)
    for (int i = 1; i <= nx; ++i) {
        // Bottom boundary row
        {
            const int j = 1;
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
        // Top boundary row
        if (ny > 1) {
            const int j = ny;
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
    }

    for (int j = 2; j <= ny - 1; ++j) {
        // Left boundary col
        {
            const int i = 1;
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
        // Right boundary col
        if (nx > 1) {
            const int i = nx;
            const double center = u_old(i, j);
            const double laplacian = rx * (u_old(i + 1, j) - 2.0 * center + u_old(i - 1, j)) +
                                     ry * (u_old(i, j + 1) - 2.0 * center + u_old(i, j - 1));
            u_new(i, j) = center + laplacian;
        }
    }
}

void MpiSolver::step3DNoOverlap(Grid3D& u_old, Grid3D& u_new, HaloExchange3D& halo,
                                const Decomposition3D& decomp, BoundaryType bc, double t) const {
    halo.exchange(u_old);
    applyPhysicalBoundary3D(u_old, decomp, bc, t);

    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const int nz = decomp.localNz();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double dz = 1.0 / static_cast<double>(m_config.nz);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);
    const double rz = m_config.alpha * m_config.dt / (dz * dz);

    #pragma omp parallel for collapse(3) default(none) \
        shared(u_old, u_new, nx, ny, nz, rx, ry, rz) schedule(static)
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

void MpiSolver::step3DOverlap(Grid3D& u_old, Grid3D& u_new, HaloExchange3D& halo,
                              const Decomposition3D& decomp, BoundaryType bc, double t) const {
    const int nx = decomp.localNx();
    const int ny = decomp.localNy();
    const int nz = decomp.localNz();
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double dz = 1.0 / static_cast<double>(m_config.nz);
    const double rx = m_config.alpha * m_config.dt / (dx * dx);
    const double ry = m_config.alpha * m_config.dt / (dy * dy);
    const double rz = m_config.alpha * m_config.dt / (dz * dz);

    // 1. Post halo exchanges
    halo.startExchange(u_old);

    // 2. Interior computation
    #pragma omp parallel for collapse(3) default(none) \
        shared(u_old, u_new, nx, ny, nz, rx, ry, rz) schedule(static)
    for (int k = 2; k <= nz - 1; ++k) {
        for (int j = 2; j <= ny - 1; ++j) {
            for (int i = 2; i <= nx - 1; ++i) {
                const double center = u_old(i, j, k);
                const double laplacian =
                    rx * (u_old(i + 1, j, k) - 2.0 * center + u_old(i - 1, j, k)) +
                    ry * (u_old(i, j + 1, k) - 2.0 * center + u_old(i, j - 1, k)) +
                    rz * (u_old(i, j, k + 1) - 2.0 * center + u_old(i, j, k - 1));
                u_new(i, j, k) = center + laplacian;
            }
        }
    }

    // 3. Complete halo exchange
    halo.finishExchange(u_old);
    applyPhysicalBoundary3D(u_old, decomp, bc, t);

    // 4. Shell computation (all boundary faces)
    auto update_cell = [&](int i, int j, int k) {
        const double center = u_old(i, j, k);
        const double laplacian = rx * (u_old(i + 1, j, k) - 2.0 * center + u_old(i - 1, j, k)) +
                                 ry * (u_old(i, j + 1, k) - 2.0 * center + u_old(i, j - 1, k)) +
                                 rz * (u_old(i, j, k + 1) - 2.0 * center + u_old(i, j, k - 1));
        u_new(i, j, k) = center + laplacian;
    };

    // Z boundary planes: k = 1 and k = nz
    for (int j = 1; j <= ny; ++j) {
        for (int i = 1; i <= nx; ++i) {
            update_cell(i, j, 1);
            if (nz > 1) {
                update_cell(i, j, nz);
            }
        }
    }

    // Y boundary planes: j = 1 and j = ny (k in [2, nz-1])
    for (int k = 2; k <= nz - 1; ++k) {
        for (int i = 1; i <= nx; ++i) {
            update_cell(i, 1, k);
            if (ny > 1) {
                update_cell(i, ny, k);
            }
        }
    }

    // X boundary planes: i = 1 and i = nx (j in [2, ny-1], k in [2, nz-1])
    for (int k = 2; k <= nz - 1; ++k) {
        for (int j = 2; j <= ny - 1; ++j) {
            update_cell(1, j, k);
            if (nx > 1) {
                update_cell(nx, j, k);
            }
        }
    }
}

Grid2D MpiSolver::run2D() const {
    Decomposition2D decomp(m_config.nx, m_config.ny, m_comm);
    HaloExchange2D halo(decomp);

    Grid2D u_old(decomp.localNx(), decomp.localNy());
    Grid2D u_new(decomp.localNx(), decomp.localNy());

    const auto ic = parseInitialConditionType(m_config.initial_condition);
    const auto bc = parseBoundaryType(m_config.boundary);
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);

    // Local subdomain coordinate initialization
    for (int j = 1; j <= decomp.localNy(); ++j) {
        const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
        for (int i = 1; i <= decomp.localNx(); ++i) {
            const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
            switch (ic) {
            case InitialConditionType::Sine:
                u_old(i, j) = analyticSine2D(x, y, 0.0, m_config.alpha);
                break;
            case InitialConditionType::Gaussian: {
                const double rx = x - 0.5;
                const double ry = y - 0.5;
                u_old(i, j) = std::exp(-(rx * rx + ry * ry) / 0.02);
                break;
            }
            case InitialConditionType::Step:
                u_old(i, j) = (x >= 0.25 && x <= 0.75 && y >= 0.25 && y <= 0.75) ? 1.0 : 0.0;
                break;
            }
        }
    }

    double t = 0.0;
    for (int step = 0; step < m_config.steps; ++step) {
        if (m_config.overlap) {
            step2DOverlap(u_old, u_new, halo, decomp, bc, t);
        } else {
            step2DNoOverlap(u_old, u_new, halo, decomp, bc, t);
        }
        t += m_config.dt;
        u_old.swap(u_new);
    }

    halo.exchange(u_old);
    applyPhysicalBoundary2D(u_old, decomp, bc, t);
    return u_old;
}

Grid3D MpiSolver::run3D() const {
    Decomposition3D decomp(m_config.nx, m_config.ny, m_config.nz, m_comm);
    HaloExchange3D halo(decomp);

    Grid3D u_old(decomp.localNx(), decomp.localNy(), decomp.localNz());
    Grid3D u_new(decomp.localNx(), decomp.localNy(), decomp.localNz());

    const auto ic = parseInitialConditionType(m_config.initial_condition);
    const auto bc = parseBoundaryType(m_config.boundary);
    const double dx = 1.0 / static_cast<double>(m_config.nx);
    const double dy = 1.0 / static_cast<double>(m_config.ny);
    const double dz = 1.0 / static_cast<double>(m_config.nz);

    for (int k = 1; k <= decomp.localNz(); ++k) {
        const double z = (static_cast<double>(decomp.startZ() + k) - 0.5) * dz;
        for (int j = 1; j <= decomp.localNy(); ++j) {
            const double y = (static_cast<double>(decomp.startY() + j) - 0.5) * dy;
            for (int i = 1; i <= decomp.localNx(); ++i) {
                const double x = (static_cast<double>(decomp.startX() + i) - 0.5) * dx;
                switch (ic) {
                case InitialConditionType::Sine:
                    u_old(i, j, k) = analyticSine3D(x, y, z, 0.0, m_config.alpha);
                    break;
                case InitialConditionType::Gaussian: {
                    const double rx = x - 0.5;
                    const double ry = y - 0.5;
                    const double rz = z - 0.5;
                    u_old(i, j, k) = std::exp(-(rx * rx + ry * ry + rz * rz) / 0.02);
                    break;
                }
                case InitialConditionType::Step:
                    u_old(i, j, k) =
                        (x >= 0.25 && x <= 0.75 && y >= 0.25 && y <= 0.75 && z >= 0.25 && z <= 0.75)
                            ? 1.0
                            : 0.0;
                    break;
                }
            }
        }
    }

    double t = 0.0;
    for (int step = 0; step < m_config.steps; ++step) {
        if (m_config.overlap) {
            step3DOverlap(u_old, u_new, halo, decomp, bc, t);
        } else {
            step3DNoOverlap(u_old, u_new, halo, decomp, bc, t);
        }
        t += m_config.dt;
        u_old.swap(u_new);
    }

    halo.exchange(u_old);
    applyPhysicalBoundary3D(u_old, decomp, bc, t);
    return u_old;
}

Grid2D MpiSolver::gather2D(const Grid2D& local_grid, const Decomposition2D& decomp) const {
    Grid2D global_grid(m_config.nx, m_config.ny);
    const int rank = decomp.rank();
    const int size = decomp.size();

    // Pack local interior cells
    std::vector<double> local_buf(decomp.localNx() * decomp.localNy());
    size_t idx = 0;
    for (int j = 1; j <= decomp.localNy(); ++j) {
        for (int i = 1; i <= decomp.localNx(); ++i) {
            local_buf[idx++] = local_grid(i, j);
        }
    }

    if (rank == 0) {
        // Place Rank 0's own data
        idx = 0;
        for (int j = 1; j <= decomp.localNy(); ++j) {
            for (int i = 1; i <= decomp.localNx(); ++i) {
                global_grid(decomp.startX() + i, decomp.startY() + j) = local_buf[idx++];
            }
        }

        // Receive from all other ranks
        for (int r = 1; r < size; ++r) {
            std::array<int, 2> r_coords{0, 0};
            MPI_Cart_coords(decomp.cartComm(), r, 2, r_coords.data());
            int r_local_nx = 0, r_start_x = 0;
            int r_local_ny = 0, r_start_y = 0;
            const int base_x = m_config.nx / decomp.dims()[0];
            const int rem_x = m_config.nx % decomp.dims()[0];
            r_local_nx = (r_coords[0] < rem_x) ? (base_x + 1) : base_x;
            r_start_x = (r_coords[0] < rem_x)
                            ? (r_coords[0] * (base_x + 1))
                            : (rem_x * (base_x + 1) + (r_coords[0] - rem_x) * base_x);

            const int base_y = m_config.ny / decomp.dims()[1];
            const int rem_y = m_config.ny % decomp.dims()[1];
            r_local_ny = (r_coords[1] < rem_y) ? (base_y + 1) : base_y;
            r_start_y = (r_coords[1] < rem_y)
                            ? (r_coords[1] * (base_y + 1))
                            : (rem_y * (base_y + 1) + (r_coords[1] - rem_y) * base_y);

            std::vector<double> recv_buf(r_local_nx * r_local_ny);
            MPI_Recv(recv_buf.data(), static_cast<int>(recv_buf.size()), MPI_DOUBLE, r, 500 + r,
                     decomp.cartComm(), MPI_STATUS_IGNORE);

            idx = 0;
            for (int j = 1; j <= r_local_ny; ++j) {
                for (int i = 1; i <= r_local_nx; ++i) {
                    global_grid(r_start_x + i, r_start_y + j) = recv_buf[idx++];
                }
            }
        }
    } else {
        MPI_Send(local_buf.data(), static_cast<int>(local_buf.size()), MPI_DOUBLE, 0, 500 + rank,
                 decomp.cartComm());
    }

    return global_grid;
}

Grid3D MpiSolver::gather3D(const Grid3D& local_grid, const Decomposition3D& decomp) const {
    Grid3D global_grid(m_config.nx, m_config.ny, m_config.nz);
    const int rank = decomp.rank();
    const int size = decomp.size();

    std::vector<double> local_buf(decomp.localNx() * decomp.localNy() * decomp.localNz());
    size_t idx = 0;
    for (int k = 1; k <= decomp.localNz(); ++k) {
        for (int j = 1; j <= decomp.localNy(); ++j) {
            for (int i = 1; i <= decomp.localNx(); ++i) {
                local_buf[idx++] = local_grid(i, j, k);
            }
        }
    }

    if (rank == 0) {
        idx = 0;
        for (int k = 1; k <= decomp.localNz(); ++k) {
            for (int j = 1; j <= decomp.localNy(); ++j) {
                for (int i = 1; i <= decomp.localNx(); ++i) {
                    global_grid(decomp.startX() + i, decomp.startY() + j, decomp.startZ() + k) =
                        local_buf[idx++];
                }
            }
        }

        for (int r = 1; r < size; ++r) {
            std::array<int, 3> r_coords{0, 0, 0};
            MPI_Cart_coords(decomp.cartComm(), r, 3, r_coords.data());

            auto calc_dist = [](int n_global, int p, int coord, int& local_n, int& start) {
                const int base = n_global / p;
                const int rem = n_global % p;
                if (coord < rem) {
                    local_n = base + 1;
                    start = coord * (base + 1);
                } else {
                    local_n = base;
                    start = rem * (base + 1) + (coord - rem) * base;
                }
            };

            int r_local_nx = 0, r_start_x = 0;
            int r_local_ny = 0, r_start_y = 0;
            int r_local_nz = 0, r_start_z = 0;
            calc_dist(m_config.nx, decomp.dims()[0], r_coords[0], r_local_nx, r_start_x);
            calc_dist(m_config.ny, decomp.dims()[1], r_coords[1], r_local_ny, r_start_y);
            calc_dist(m_config.nz, decomp.dims()[2], r_coords[2], r_local_nz, r_start_z);

            std::vector<double> recv_buf(r_local_nx * r_local_ny * r_local_nz);
            MPI_Recv(recv_buf.data(), static_cast<int>(recv_buf.size()), MPI_DOUBLE, r, 600 + r,
                     decomp.cartComm(), MPI_STATUS_IGNORE);

            idx = 0;
            for (int k = 1; k <= r_local_nz; ++k) {
                for (int j = 1; j <= r_local_ny; ++j) {
                    for (int i = 1; i <= r_local_nx; ++i) {
                        global_grid(r_start_x + i, r_start_y + j, r_start_z + k) = recv_buf[idx++];
                    }
                }
            }
        }
    } else {
        MPI_Send(local_buf.data(), static_cast<int>(local_buf.size()), MPI_DOUBLE, 0, 600 + rank,
                 decomp.cartComm());
    }

    return global_grid;
}

} // namespace heat
