#include <heat/halo.hpp>

#include <iostream>

namespace heat {

HaloExchange2D::HaloExchange2D(const Decomposition2D& decomp)
    : m_decomp(decomp)
    , m_send_left(decomp.localNy())
    , m_send_right(decomp.localNy())
    , m_send_bottom(decomp.localNx())
    , m_send_top(decomp.localNx())
    , m_recv_left(decomp.localNy())
    , m_recv_right(decomp.localNy())
    , m_recv_bottom(decomp.localNx())
    , m_recv_top(decomp.localNx()) {
    m_requests.reserve(8);
}

void HaloExchange2D::startExchange(Grid2D& grid) {
    m_requests.clear();
    const int nx = m_decomp.localNx();
    const int ny = m_decomp.localNy();
    MPI_Comm comm = m_decomp.cartComm();

    // 1. Post receives
    // From Left
    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_left.data(), ny, MPI_DOUBLE, m_decomp.neighborLeft(), 100, comm, &req);
        m_requests.push_back(req);
    }
    // From Right
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_right.data(), ny, MPI_DOUBLE, m_decomp.neighborRight(), 101, comm, &req);
        m_requests.push_back(req);
    }
    // From Bottom
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_bottom.data(), nx, MPI_DOUBLE, m_decomp.neighborBottom(), 102, comm, &req);
        m_requests.push_back(req);
    }
    // From Top
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_top.data(), nx, MPI_DOUBLE, m_decomp.neighborTop(), 103, comm, &req);
        m_requests.push_back(req);
    }

    // 2. Pack and post sends
    // Send to Left (from col 1)
    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            m_send_left[j - 1] = grid(1, j);
        }
        MPI_Request req;
        MPI_Isend(m_send_left.data(), ny, MPI_DOUBLE, m_decomp.neighborLeft(), 101, comm, &req);
        m_requests.push_back(req);
    }
    // Send to Right (from col nx)
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            m_send_right[j - 1] = grid(nx, j);
        }
        MPI_Request req;
        MPI_Isend(m_send_right.data(), ny, MPI_DOUBLE, m_decomp.neighborRight(), 100, comm, &req);
        m_requests.push_back(req);
    }
    // Send to Bottom (from row 1)
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        for (int i = 1; i <= nx; ++i) {
            m_send_bottom[i - 1] = grid(i, 1);
        }
        MPI_Request req;
        MPI_Isend(m_send_bottom.data(), nx, MPI_DOUBLE, m_decomp.neighborBottom(), 103, comm, &req);
        m_requests.push_back(req);
    }
    // Send to Top (from row ny)
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        for (int i = 1; i <= nx; ++i) {
            m_send_top[i - 1] = grid(i, ny);
        }
        MPI_Request req;
        MPI_Isend(m_send_top.data(), nx, MPI_DOUBLE, m_decomp.neighborTop(), 102, comm, &req);
        m_requests.push_back(req);
    }
}

void HaloExchange2D::finishExchange(Grid2D& grid) {
    if (!m_requests.empty()) {
        MPI_Waitall(static_cast<int>(m_requests.size()), m_requests.data(), MPI_STATUSES_IGNORE);
    }

    const int nx = m_decomp.localNx();
    const int ny = m_decomp.localNy();

    // Unpack received halos into ghost cells
    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            grid(0, j) = m_recv_left[j - 1];
        }
    }
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        for (int j = 1; j <= ny; ++j) {
            grid(nx + 1, j) = m_recv_right[j - 1];
        }
    }
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        for (int i = 1; i <= nx; ++i) {
            grid(i, 0) = m_recv_bottom[i - 1];
        }
    }
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        for (int i = 1; i <= nx; ++i) {
            grid(i, ny + 1) = m_recv_top[i - 1];
        }
    }
}

void HaloExchange2D::exchange(Grid2D& grid) {
    startExchange(grid);
    finishExchange(grid);
}

// ----------------- HaloExchange3D -----------------

HaloExchange3D::HaloExchange3D(const Decomposition3D& decomp)
    : m_decomp(decomp)
    , m_send_left(decomp.localNy() * decomp.localNz())
    , m_send_right(decomp.localNy() * decomp.localNz())
    , m_send_bottom(decomp.localNx() * decomp.localNz())
    , m_send_top(decomp.localNx() * decomp.localNz())
    , m_send_back(decomp.localNx() * decomp.localNy())
    , m_send_front(decomp.localNx() * decomp.localNy())
    , m_recv_left(decomp.localNy() * decomp.localNz())
    , m_recv_right(decomp.localNy() * decomp.localNz())
    , m_recv_bottom(decomp.localNx() * decomp.localNz())
    , m_recv_top(decomp.localNx() * decomp.localNz())
    , m_recv_back(decomp.localNx() * decomp.localNy())
    , m_recv_front(decomp.localNx() * decomp.localNy()) {
    m_requests.reserve(12);
}

void HaloExchange3D::startExchange(Grid3D& grid) {
    m_requests.clear();
    const int nx = m_decomp.localNx();
    const int ny = m_decomp.localNy();
    const int nz = m_decomp.localNz();
    MPI_Comm comm = m_decomp.cartComm();

    const int size_x_face = ny * nz;
    const int size_y_face = nx * nz;
    const int size_z_face = nx * ny;

    // Receives
    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_left.data(), size_x_face, MPI_DOUBLE, m_decomp.neighborLeft(), 200, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_right.data(), size_x_face, MPI_DOUBLE, m_decomp.neighborRight(), 201, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_bottom.data(), size_y_face, MPI_DOUBLE, m_decomp.neighborBottom(), 202,
                  comm, &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_top.data(), size_y_face, MPI_DOUBLE, m_decomp.neighborTop(), 203, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborBack() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_back.data(), size_z_face, MPI_DOUBLE, m_decomp.neighborBack(), 204, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborFront() != MPI_PROC_NULL) {
        MPI_Request req;
        MPI_Irecv(m_recv_front.data(), size_z_face, MPI_DOUBLE, m_decomp.neighborFront(), 205, comm,
                  &req);
        m_requests.push_back(req);
    }

    // Sends
    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int j = 1; j <= ny; ++j) {
                m_send_left[idx++] = grid(1, j, k);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_left.data(), size_x_face, MPI_DOUBLE, m_decomp.neighborLeft(), 201, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int j = 1; j <= ny; ++j) {
                m_send_right[idx++] = grid(nx, j, k);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_right.data(), size_x_face, MPI_DOUBLE, m_decomp.neighborRight(), 200, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int i = 1; i <= nx; ++i) {
                m_send_bottom[idx++] = grid(i, 1, k);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_bottom.data(), size_y_face, MPI_DOUBLE, m_decomp.neighborBottom(), 203,
                  comm, &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int i = 1; i <= nx; ++i) {
                m_send_top[idx++] = grid(i, ny, k);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_top.data(), size_y_face, MPI_DOUBLE, m_decomp.neighborTop(), 202, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborBack() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                m_send_back[idx++] = grid(i, j, 1);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_back.data(), size_z_face, MPI_DOUBLE, m_decomp.neighborBack(), 205, comm,
                  &req);
        m_requests.push_back(req);
    }
    if (m_decomp.neighborFront() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                m_send_front[idx++] = grid(i, j, nz);
            }
        }
        MPI_Request req;
        MPI_Isend(m_send_front.data(), size_z_face, MPI_DOUBLE, m_decomp.neighborFront(), 204, comm,
                  &req);
        m_requests.push_back(req);
    }
}

void HaloExchange3D::finishExchange(Grid3D& grid) {
    if (!m_requests.empty()) {
        MPI_Waitall(static_cast<int>(m_requests.size()), m_requests.data(), MPI_STATUSES_IGNORE);
    }

    const int nx = m_decomp.localNx();
    const int ny = m_decomp.localNy();
    const int nz = m_decomp.localNz();

    if (m_decomp.neighborLeft() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int j = 1; j <= ny; ++j) {
                grid(0, j, k) = m_recv_left[idx++];
            }
        }
    }
    if (m_decomp.neighborRight() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int j = 1; j <= ny; ++j) {
                grid(nx + 1, j, k) = m_recv_right[idx++];
            }
        }
    }
    if (m_decomp.neighborBottom() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, 0, k) = m_recv_bottom[idx++];
            }
        }
    }
    if (m_decomp.neighborTop() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int k = 1; k <= nz; ++k) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, ny + 1, k) = m_recv_top[idx++];
            }
        }
    }
    if (m_decomp.neighborBack() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, j, 0) = m_recv_back[idx++];
            }
        }
    }
    if (m_decomp.neighborFront() != MPI_PROC_NULL) {
        size_t idx = 0;
        for (int j = 1; j <= ny; ++j) {
            for (int i = 1; i <= nx; ++i) {
                grid(i, j, nz + 1) = m_recv_front[idx++];
            }
        }
    }
}

void HaloExchange3D::exchange(Grid3D& grid) {
    startExchange(grid);
    finishExchange(grid);
}

} // namespace heat
