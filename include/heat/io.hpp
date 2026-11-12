#pragma once

#include <heat/config.hpp>
#include <heat/decomposition.hpp>
#include <heat/grid.hpp>

#include <mpi.h>

#include <array>
#include <string>
#include <utility>
#include <vector>

namespace heat {

/**
 * @brief Parallel VTK ImageData writer (one .vti piece per rank + a .pvti master per snapshot
 * + a .pvd time series), openable directly in ParaView.
 *
 * Temperature is written as cell data (Float64, appended raw binary). Each rank writes only its
 * own interior cells, so no gather onto rank 0 is needed and I/O scales with the rank count.
 */
class VtkWriter {
  public:
    VtkWriter(const Config& config, MPI_Comm comm);

    void write2D(const Decomposition2D& decomp, const Grid2D& grid, int step, double t);
    void write3D(const Decomposition3D& decomp, const Grid3D& grid, int step, double t);

    [[nodiscard]] int snapshotsWritten() const noexcept {
        return static_cast<int>(m_entries.size());
    }

  private:
    Config m_config;
    MPI_Comm m_comm;
    int m_rank = 0;
    std::string m_dir;
    std::vector<std::pair<double, std::string>> m_entries; ///< (time, master file name)

    void writePiece(const std::string& file, const std::array<int, 6>& extent,
                    const std::vector<double>& cells) const;
    void writeMaster(const std::string& master, const std::vector<int>& all_extents, int nranks,
                     int step) const;
    void writePvd() const;
    [[nodiscard]] std::string stem(int step) const;
};

/**
 * @brief Per-rank binary checkpoint / restart.
 *
 * Each rank writes `<prefix>_r<rank>.bin` (atomically, via a temporary file + rename) holding a
 * header (dimension, global size, rank count, local extents, completed step, time) followed by its
 * interior cells. Restart validates the header against the current configuration and requires the
 * same rank count; all ranks must agree on the step. Because the explicit scheme depends only on
 * the previous step and ghost cells are rebuilt by the first exchange, a restarted run is
 * bit-identical to an uninterrupted one.
 */
void writeCheckpoint2D(const Config& config, const Decomposition2D& decomp, const Grid2D& grid,
                       int step, double t);
void writeCheckpoint3D(const Config& config, const Decomposition3D& decomp, const Grid3D& grid,
                       int step, double t);

/// Loads the checkpoint into the interior of `grid`; throws std::runtime_error on any mismatch.
void readCheckpoint2D(const Config& config, const Decomposition2D& decomp, Grid2D& grid, int& step,
                      double& t);
void readCheckpoint3D(const Config& config, const Decomposition3D& decomp, Grid3D& grid, int& step,
                      double& t);

} // namespace heat
