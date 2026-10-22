#pragma once

#include <cstddef>
#include <vector>

namespace heat {

/**
 * @brief Contiguous 1D grid with 1-cell halo on each boundary.
 *
 * Physical cells: [1, nx].
 * Memory size: nx + 2.
 */
class Grid1D {
  public:
    explicit Grid1D(int nx, double init_val = 0.0)
        : m_nx(nx)
        , m_data(static_cast<size_t>(nx + 2), init_val) {}

    [[nodiscard]] int nx() const noexcept { return m_nx; }
    [[nodiscard]] size_t totalCells() const noexcept { return m_data.size(); }

    [[nodiscard]] double& operator()(int x) noexcept { return m_data[static_cast<size_t>(x)]; }

    [[nodiscard]] double operator()(int x) const noexcept { return m_data[static_cast<size_t>(x)]; }

    [[nodiscard]] double* data() noexcept { return m_data.data(); }
    [[nodiscard]] const double* data() const noexcept { return m_data.data(); }

    void swap(Grid1D& other) noexcept { m_data.swap(other.m_data); }

  private:
    int m_nx;
    std::vector<double> m_data;
};

/**
 * @brief Contiguous 2D grid with 1-cell halo on each boundary.
 *
 * Physical cells: [1, nx] x [1, ny].
 * Memory size: (nx + 2) * (ny + 2).
 * Row-major ordering: index(x, y) = y * stride_x + x.
 */
class Grid2D {
  public:
    Grid2D(int nx, int ny, double init_val = 0.0)
        : m_nx(nx)
        , m_ny(ny)
        , m_stride_x(nx + 2)
        , m_data(static_cast<size_t>(nx + 2) * static_cast<size_t>(ny + 2), init_val) {}

    [[nodiscard]] int nx() const noexcept { return m_nx; }
    [[nodiscard]] int ny() const noexcept { return m_ny; }
    [[nodiscard]] int strideX() const noexcept { return m_stride_x; }
    [[nodiscard]] size_t totalCells() const noexcept { return m_data.size(); }

    [[nodiscard]] size_t index(int x, int y) const noexcept {
        return static_cast<size_t>(y) * static_cast<size_t>(m_stride_x) + static_cast<size_t>(x);
    }

    [[nodiscard]] double& operator()(int x, int y) noexcept { return m_data[index(x, y)]; }

    [[nodiscard]] double operator()(int x, int y) const noexcept { return m_data[index(x, y)]; }

    [[nodiscard]] double* data() noexcept { return m_data.data(); }
    [[nodiscard]] const double* data() const noexcept { return m_data.data(); }

    void swap(Grid2D& other) noexcept { m_data.swap(other.m_data); }

  private:
    int m_nx;
    int m_ny;
    int m_stride_x;
    std::vector<double> m_data;
};

/**
 * @brief Contiguous 3D grid with 1-cell halo on each boundary.
 *
 * Physical cells: [1, nx] x [1, ny] x [1, nz].
 * Memory size: (nx + 2) * (ny + 2) * (nz + 2).
 * Row-major ordering: index(x, y, z) = z * (stride_y * stride_x) + y * stride_x + x.
 */
class Grid3D {
  public:
    Grid3D(int nx, int ny, int nz, double init_val = 0.0)
        : m_nx(nx)
        , m_ny(ny)
        , m_nz(nz)
        , m_stride_x(nx + 2)
        , m_stride_y(ny + 2)
        , m_data(static_cast<size_t>(nx + 2) * static_cast<size_t>(ny + 2) *
                     static_cast<size_t>(nz + 2),
                 init_val) {}

    [[nodiscard]] int nx() const noexcept { return m_nx; }
    [[nodiscard]] int ny() const noexcept { return m_ny; }
    [[nodiscard]] int nz() const noexcept { return m_nz; }
    [[nodiscard]] int strideX() const noexcept { return m_stride_x; }
    [[nodiscard]] int strideY() const noexcept { return m_stride_y; }
    [[nodiscard]] size_t totalCells() const noexcept { return m_data.size(); }

    [[nodiscard]] size_t index(int x, int y, int z) const noexcept {
        return (static_cast<size_t>(z) * static_cast<size_t>(m_stride_x * m_stride_y)) +
               (static_cast<size_t>(y) * static_cast<size_t>(m_stride_x)) + static_cast<size_t>(x);
    }

    [[nodiscard]] double& operator()(int x, int y, int z) noexcept {
        return m_data[index(x, y, z)];
    }

    [[nodiscard]] double operator()(int x, int y, int z) const noexcept {
        return m_data[index(x, y, z)];
    }

    [[nodiscard]] double* data() noexcept { return m_data.data(); }
    [[nodiscard]] const double* data() const noexcept { return m_data.data(); }

    void swap(Grid3D& other) noexcept { m_data.swap(other.m_data); }

  private:
    int m_nx;
    int m_ny;
    int m_nz;
    int m_stride_x;
    int m_stride_y;
    std::vector<double> m_data;
};

} // namespace heat
