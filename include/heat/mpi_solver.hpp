#pragma once

#include <heat/boundary.hpp>
#include <heat/config.hpp>
#include <heat/decomposition.hpp>
#include <heat/grid.hpp>
#include <heat/halo.hpp>

namespace heat {

/**
 * @brief Distributed-memory MPI solver for 2D & 3D heat diffusion.
 * Supports compute-communication overlap.
 */
class MpiSolver {
  public:
    explicit MpiSolver(Config config, MPI_Comm comm = MPI_COMM_WORLD);

    /**
     * @brief Executes full 2D distributed simulation and returns local subdomain grid.
     */
    [[nodiscard]] Grid2D run2D() const;

    /**
     * @brief Executes full 3D distributed simulation and returns local subdomain grid.
     */
    [[nodiscard]] Grid3D run3D() const;

    /**
     * @brief Gathers 2D distributed grid from all ranks onto Rank 0 (for equivalence testing &
     * output).
     */
    [[nodiscard]] Grid2D gather2D(const Grid2D& local_grid, const Decomposition2D& decomp) const;

    /**
     * @brief Gathers 3D distributed grid from all ranks onto Rank 0.
     */
    [[nodiscard]] Grid3D gather3D(const Grid3D& local_grid, const Decomposition3D& decomp) const;

    [[nodiscard]] const Config& config() const noexcept { return m_config; }

  private:
    Config m_config;
    MPI_Comm m_comm;

    void step2DOverlap(Grid2D& u_old, Grid2D& u_new, HaloExchange2D& halo,
                       const Decomposition2D& decomp, BoundaryType bc, double t) const;

    void step2DNoOverlap(Grid2D& u_old, Grid2D& u_new, HaloExchange2D& halo,
                         const Decomposition2D& decomp, BoundaryType bc, double t) const;

    void step3DOverlap(Grid3D& u_old, Grid3D& u_new, HaloExchange3D& halo,
                       const Decomposition3D& decomp, BoundaryType bc, double t) const;

    void step3DNoOverlap(Grid3D& u_old, Grid3D& u_new, HaloExchange3D& halo,
                         const Decomposition3D& decomp, BoundaryType bc, double t) const;

    void applyPhysicalBoundary2D(Grid2D& grid, const Decomposition2D& decomp, BoundaryType bc,
                                 double t) const;

    void applyPhysicalBoundary3D(Grid3D& grid, const Decomposition3D& decomp, BoundaryType bc,
                                 double t) const;
};

} // namespace heat
