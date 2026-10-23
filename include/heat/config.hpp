#pragma once

#include <string>

namespace heat {

/**
 * @brief Configuration parameters for the Heat Equation solver.
 */
struct Config {
    int dimension = 2;                      ///< 2 for 2D, 3 for 3D
    int nx = 64;                            ///< Grid size along X (excluding halo)
    int ny = 64;                            ///< Grid size along Y (excluding halo)
    int nz = 64;                            ///< Grid size along Z (excluding halo, 3D only)
    double alpha = 1.0;                     ///< Thermal diffusivity
    double dt = 0.0001;                     ///< Time step size
    int steps = 100;                        ///< Number of time integration steps
    std::string boundary = "dirichlet";     ///< "dirichlet" or "neumann"
    std::string initial_condition = "sine"; ///< "sine", "gaussian", "step"
    bool overlap = false;                   ///< Enable compute-communication overlap
    int snapshot_freq = 0;                  ///< Snapshot write frequency (0 = disabled)
    std::string output_path = "output/";    ///< Directory for simulation snapshots

    /**
     * @brief Validates explicit FTCS stability condition:
     * dt <= 1 / (2 * alpha * sum(1/dx_i^2))
     * @return true if configuration is numerically stable, false otherwise.
     */
    [[nodiscard]] bool isStable() const;

    /**
     * @brief Calculates maximum allowable dt for numerical stability.
     */
    [[nodiscard]] double maxStableDt() const;
};

/**
 * @brief Simple key=value configuration file parser.
 */
class ConfigParser {
  public:
    static Config loadFromFile(const std::string& filepath);
    static Config loadFromCommandLine(int argc, char** argv, const Config& base = Config{});
};

} // namespace heat
