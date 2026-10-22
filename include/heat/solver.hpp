#pragma once

#include <heat/boundary.hpp>
#include <heat/config.hpp>
#include <heat/grid.hpp>

namespace heat {

/**
 * @brief Explicit Forward-Time Central-Space (FTCS) serial solver for 2D & 3D heat diffusion.
 */
class SerialSolver {
  public:
    explicit SerialSolver(const Config& config);

    /**
     * @brief Executes single FTCS step for 2D grid.
     */
    void step2D(const Grid2D& u_old, Grid2D& u_new);

    /**
     * @brief Executes single FTCS step for 3D grid.
     */
    void step3D(const Grid3D& u_old, Grid3D& u_new);

    /**
     * @brief Runs full 2D simulation and returns final grid.
     */
    Grid2D run2D();

    /**
     * @brief Runs full 3D simulation and returns final grid.
     */
    Grid3D run3D();

    [[nodiscard]] const Config& config() const noexcept { return m_config; }

  private:
    Config m_config;
};

} // namespace heat
