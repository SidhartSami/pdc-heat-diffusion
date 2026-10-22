#pragma once

#include <heat/decomposition.hpp>
#include <heat/grid.hpp>

#include <mpi.h>

#include <vector>

namespace heat {

/**
 * @brief Non-blocking halo exchange manager for 2D subdomains.
 */
class HaloExchange2D {
  public:
    explicit HaloExchange2D(const Decomposition2D& decomp);

    /**
     * @brief Posts non-blocking Irecv and Isend for all four Cartesian faces.
     */
    void startExchange(Grid2D& grid);

    /**
     * @brief Waits for exchanges to complete and unpacks ghost layers.
     */
    void finishExchange(Grid2D& grid);

    /**
     * @brief Convenience blocking exchange (startExchange + finishExchange).
     */
    void exchange(Grid2D& grid);

  private:
    const Decomposition2D& m_decomp;
    std::vector<double> m_send_left;
    std::vector<double> m_send_right;
    std::vector<double> m_send_bottom;
    std::vector<double> m_send_top;

    std::vector<double> m_recv_left;
    std::vector<double> m_recv_right;
    std::vector<double> m_recv_bottom;
    std::vector<double> m_recv_top;

    std::vector<MPI_Request> m_requests;
};

/**
 * @brief Non-blocking halo exchange manager for 3D subdomains.
 */
class HaloExchange3D {
  public:
    explicit HaloExchange3D(const Decomposition3D& decomp);

    void startExchange(Grid3D& grid);
    void finishExchange(Grid3D& grid);
    void exchange(Grid3D& grid);

  private:
    const Decomposition3D& m_decomp;
    std::vector<double> m_send_left;
    std::vector<double> m_send_right;
    std::vector<double> m_send_bottom;
    std::vector<double> m_send_top;
    std::vector<double> m_send_back;
    std::vector<double> m_send_front;

    std::vector<double> m_recv_left;
    std::vector<double> m_recv_right;
    std::vector<double> m_recv_bottom;
    std::vector<double> m_recv_top;
    std::vector<double> m_recv_back;
    std::vector<double> m_recv_front;

    std::vector<MPI_Request> m_requests;
};

} // namespace heat
