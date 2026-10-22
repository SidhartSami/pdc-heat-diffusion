#include <heat/decomposition.hpp>

#include <iostream>
#include <stdexcept>

namespace heat {

static void computeBlockDistribution(int n_global, int p, int coord, int& local_n, int& start) {
    const int base = n_global / p;
    const int rem = n_global % p;

    if (coord < rem) {
        local_n = base + 1;
        start = coord * (base + 1);
    } else {
        local_n = base;
        start = rem * (base + 1) + (coord - rem) * base;
    }
}

Decomposition2D::Decomposition2D(int global_nx, int global_ny, MPI_Comm comm)
    : m_global_nx(global_nx)
    , m_global_ny(global_ny) {
    MPI_Comm_size(comm, &m_size);

    m_dims[0] = 0;
    m_dims[1] = 0;
    MPI_Dims_create(m_size, 2, m_dims.data());

    const int periods[2] = {0, 0}; // Non-periodic domain
    const int reorder = 1;
    MPI_Cart_create(comm, 2, m_dims.data(), periods, reorder, &m_cart_comm);

    MPI_Comm_rank(m_cart_comm, &m_rank);
    MPI_Cart_coords(m_cart_comm, m_rank, 2, m_coords.data());

    // Coordinate 0 is X dimension, Coordinate 1 is Y dimension
    computeBlockDistribution(m_global_nx, m_dims[0], m_coords[0], m_local_nx, m_start_x);
    computeBlockDistribution(m_global_ny, m_dims[1], m_coords[1], m_local_ny, m_start_y);

    // Shifts: direction 0 (X: left/right), direction 1 (Y: bottom/top)
    MPI_Cart_shift(m_cart_comm, 0, 1, &m_nbr_left, &m_nbr_right);
    MPI_Cart_shift(m_cart_comm, 1, 1, &m_nbr_bottom, &m_nbr_top);
}

Decomposition2D::~Decomposition2D() {
    if (m_cart_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&m_cart_comm);
    }
}

Decomposition2D::Decomposition2D(Decomposition2D&& other) noexcept
    : m_global_nx(other.m_global_nx)
    , m_global_ny(other.m_global_ny)
    , m_local_nx(other.m_local_nx)
    , m_local_ny(other.m_local_ny)
    , m_start_x(other.m_start_x)
    , m_start_y(other.m_start_y)
    , m_rank(other.m_rank)
    , m_size(other.m_size)
    , m_dims(other.m_dims)
    , m_coords(other.m_coords)
    , m_nbr_left(other.m_nbr_left)
    , m_nbr_right(other.m_nbr_right)
    , m_nbr_bottom(other.m_nbr_bottom)
    , m_nbr_top(other.m_nbr_top)
    , m_cart_comm(other.m_cart_comm) {
    other.m_cart_comm = MPI_COMM_NULL;
}

Decomposition2D& Decomposition2D::operator=(Decomposition2D&& other) noexcept {
    if (this != &other) {
        if (m_cart_comm != MPI_COMM_NULL) {
            MPI_Comm_free(&m_cart_comm);
        }
        m_global_nx = other.m_global_nx;
        m_global_ny = other.m_global_ny;
        m_local_nx = other.m_local_nx;
        m_local_ny = other.m_local_ny;
        m_start_x = other.m_start_x;
        m_start_y = other.m_start_y;
        m_rank = other.m_rank;
        m_size = other.m_size;
        m_dims = other.m_dims;
        m_coords = other.m_coords;
        m_nbr_left = other.m_nbr_left;
        m_nbr_right = other.m_nbr_right;
        m_nbr_bottom = other.m_nbr_bottom;
        m_nbr_top = other.m_nbr_top;
        m_cart_comm = other.m_cart_comm;
        other.m_cart_comm = MPI_COMM_NULL;
    }
    return *this;
}

Decomposition3D::Decomposition3D(int global_nx, int global_ny, int global_nz, MPI_Comm comm)
    : m_global_nx(global_nx)
    , m_global_ny(global_ny)
    , m_global_nz(global_nz) {
    MPI_Comm_size(comm, &m_size);

    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
    MPI_Dims_create(m_size, 3, m_dims.data());

    const int periods[3] = {0, 0, 0};
    const int reorder = 1;
    MPI_Cart_create(comm, 3, m_dims.data(), periods, reorder, &m_cart_comm);

    MPI_Comm_rank(m_cart_comm, &m_rank);
    MPI_Cart_coords(m_cart_comm, m_rank, 3, m_coords.data());

    // Coordinate 0: X, Coordinate 1: Y, Coordinate 2: Z
    computeBlockDistribution(m_global_nx, m_dims[0], m_coords[0], m_local_nx, m_start_x);
    computeBlockDistribution(m_global_ny, m_dims[1], m_coords[1], m_local_ny, m_start_y);
    computeBlockDistribution(m_global_nz, m_dims[2], m_coords[2], m_local_nz, m_start_z);

    MPI_Cart_shift(m_cart_comm, 0, 1, &m_nbr_left, &m_nbr_right);
    MPI_Cart_shift(m_cart_comm, 1, 1, &m_nbr_bottom, &m_nbr_top);
    MPI_Cart_shift(m_cart_comm, 2, 1, &m_nbr_back, &m_nbr_front);
}

Decomposition3D::~Decomposition3D() {
    if (m_cart_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&m_cart_comm);
    }
}

Decomposition3D::Decomposition3D(Decomposition3D&& other) noexcept
    : m_global_nx(other.m_global_nx)
    , m_global_ny(other.m_global_ny)
    , m_global_nz(other.m_global_nz)
    , m_local_nx(other.m_local_nx)
    , m_local_ny(other.m_local_ny)
    , m_local_nz(other.m_local_nz)
    , m_start_x(other.m_start_x)
    , m_start_y(other.m_start_y)
    , m_start_z(other.m_start_z)
    , m_rank(other.m_rank)
    , m_size(other.m_size)
    , m_dims(other.m_dims)
    , m_coords(other.m_coords)
    , m_nbr_left(other.m_nbr_left)
    , m_nbr_right(other.m_nbr_right)
    , m_nbr_bottom(other.m_nbr_bottom)
    , m_nbr_top(other.m_nbr_top)
    , m_nbr_back(other.m_nbr_back)
    , m_nbr_front(other.m_nbr_front)
    , m_cart_comm(other.m_cart_comm) {
    other.m_cart_comm = MPI_COMM_NULL;
}

Decomposition3D& Decomposition3D::operator=(Decomposition3D&& other) noexcept {
    if (this != &other) {
        if (m_cart_comm != MPI_COMM_NULL) {
            MPI_Comm_free(&m_cart_comm);
        }
        m_global_nx = other.m_global_nx;
        m_global_ny = other.m_global_ny;
        m_global_nz = other.m_global_nz;
        m_local_nx = other.m_local_nx;
        m_local_ny = other.m_local_ny;
        m_local_nz = other.m_local_nz;
        m_start_x = other.m_start_x;
        m_start_y = other.m_start_y;
        m_start_z = other.m_start_z;
        m_rank = other.m_rank;
        m_size = other.m_size;
        m_dims = other.m_dims;
        m_coords = other.m_coords;
        m_nbr_left = other.m_nbr_left;
        m_nbr_right = other.m_nbr_right;
        m_nbr_bottom = other.m_nbr_bottom;
        m_nbr_top = other.m_nbr_top;
        m_nbr_back = other.m_nbr_back;
        m_nbr_front = other.m_nbr_front;
        m_cart_comm = other.m_cart_comm;
        other.m_cart_comm = MPI_COMM_NULL;
    }
    return *this;
}

} // namespace heat
