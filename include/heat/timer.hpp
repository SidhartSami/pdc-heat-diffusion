#pragma once

#include <mpi.h>

#include <string>
#include <unordered_map>

namespace heat {

/**
 * @brief High-resolution MPI-synchronized timer helper.
 */
class Timer {
  public:
    Timer() = default;

    void start();
    void stop();
    double elapsedSeconds() const;

    /**
     * @brief Computes maximum, minimum, and average duration across all MPI ranks.
     */
    static void reduceSummary(double local_time, MPI_Comm comm, double& min_time, double& max_time,
                              double& avg_time);

  private:
    double m_start_time = 0.0;
    double m_elapsed = 0.0;
    bool m_running = false;
};

} // namespace heat
