#pragma once

#include <mpi.h>

#include <array>
#include <vector>

namespace heat {

/**
 * @brief 2D Cartesian domain decomposition descriptor.
 */
class Decomposition2D {
  public:
    Decomposition2D(int global_nx, int global_ny, MPI_Comm comm = MPI_COMM_WORLD);
    ~Decomposition2D();

    Decomposition2D(const Decomposition2D&) = delete;
    Decomposition2D& operator=(const Decomposition2D&) = delete;
    Decomposition2D(Decomposition2D&& other) noexcept;
    Decomposition2D& operator=(Decomposition2D&& other) noexcept;

    [[nodiscard]] int globalNx() const noexcept { return m_global_nx; }
    [[nodiscard]] int globalNy() const noexcept { return m_global_ny; }
    [[nodiscard]] int localNx() const noexcept { return m_local_nx; }
    [[nodiscard]] int localNy() const noexcept { return m_local_ny; }
    [[nodiscard]] int startX() const noexcept { return m_start_x; }
    [[nodiscard]] int startY() const noexcept { return m_start_y; }

    [[nodiscard]] int rank() const noexcept { return m_rank; }
    [[nodiscard]] int size() const noexcept { return m_size; }
    [[nodiscard]] const std::array<int, 2>& dims() const noexcept { return m_dims; }
    [[nodiscard]] const std::array<int, 2>& coords() const noexcept { return m_coords; }

    [[nodiscard]] int neighborLeft() const noexcept { return m_nbr_left; }
    [[nodiscard]] int neighborRight() const noexcept { return m_nbr_right; }
    [[nodiscard]] int neighborBottom() const noexcept { return m_nbr_bottom; }
    [[nodiscard]] int neighborTop() const noexcept { return m_nbr_top; }

    [[nodiscard]] MPI_Comm cartComm() const noexcept { return m_cart_comm; }

  private:
    int m_global_nx;
    int m_global_ny;
    int m_local_nx;
    int m_local_ny;
    int m_start_x;
    int m_start_y;

    int m_rank = 0;
    int m_size = 1;
    std::array<int, 2> m_dims{0, 0};
    std::array<int, 2> m_coords{0, 0};

    int m_nbr_left = MPI_PROC_NULL;
    int m_nbr_right = MPI_PROC_NULL;
    int m_nbr_bottom = MPI_PROC_NULL;
    int m_nbr_top = MPI_PROC_NULL;

    MPI_Comm m_cart_comm = MPI_COMM_NULL;
};

/**
 * @brief 3D Cartesian domain decomposition descriptor.
 */
class Decomposition3D {
  public:
    Decomposition3D(int global_nx, int global_ny, int global_nz, MPI_Comm comm = MPI_COMM_WORLD);
    ~Decomposition3D();

    Decomposition3D(const Decomposition3D&) = delete;
    Decomposition3D& operator=(const Decomposition3D&) = delete;
    Decomposition3D(Decomposition3D&& other) noexcept;
    Decomposition3D& operator=(Decomposition3D&& other) noexcept;

    [[nodiscard]] int globalNx() const noexcept { return m_global_nx; }
    [[nodiscard]] int globalNy() const noexcept { return m_global_ny; }
    [[nodiscard]] int globalNz() const noexcept { return m_global_nz; }
    [[nodiscard]] int localNx() const noexcept { return m_local_nx; }
    [[nodiscard]] int localNy() const noexcept { return m_local_ny; }
    [[nodiscard]] int localNz() const noexcept { return m_local_nz; }
    [[nodiscard]] int startX() const noexcept { return m_start_x; }
    [[nodiscard]] int startY() const noexcept { return m_start_y; }
    [[nodiscard]] int startZ() const noexcept { return m_start_z; }

    [[nodiscard]] int rank() const noexcept { return m_rank; }
    [[nodiscard]] int size() const noexcept { return m_size; }
    [[nodiscard]] const std::array<int, 3>& dims() const noexcept { return m_dims; }
    [[nodiscard]] const std::array<int, 3>& coords() const noexcept { return m_coords; }

    [[nodiscard]] int neighborLeft() const noexcept { return m_nbr_left; }
    [[nodiscard]] int neighborRight() const noexcept { return m_nbr_right; }
    [[nodiscard]] int neighborBottom() const noexcept { return m_nbr_bottom; }
    [[nodiscard]] int neighborTop() const noexcept { return m_nbr_top; }
    [[nodiscard]] int neighborBack() const noexcept { return m_nbr_back; }
    [[nodiscard]] int neighborFront() const noexcept { return m_nbr_front; }

    [[nodiscard]] MPI_Comm cartComm() const noexcept { return m_cart_comm; }

  private:
    int m_global_nx;
    int m_global_ny;
    int m_global_nz;
    int m_local_nx;
    int m_local_ny;
    int m_local_nz;
    int m_start_x;
    int m_start_y;
    int m_start_z;

    int m_rank = 0;
    int m_size = 1;
    std::array<int, 3> m_dims{0, 0, 0};
    std::array<int, 3> m_coords{0, 0, 0};

    int m_nbr_left = MPI_PROC_NULL;
    int m_nbr_right = MPI_PROC_NULL;
    int m_nbr_bottom = MPI_PROC_NULL;
    int m_nbr_top = MPI_PROC_NULL;
    int m_nbr_back = MPI_PROC_NULL;
    int m_nbr_front = MPI_PROC_NULL;

    MPI_Comm m_cart_comm = MPI_COMM_NULL;
};

} // namespace heat
