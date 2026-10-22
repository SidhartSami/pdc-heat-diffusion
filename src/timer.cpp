#include <heat/timer.hpp>

namespace heat {

void Timer::start() {
    m_running = true;
    m_start_time = MPI_Wtime();
}

void Timer::stop() {
    if (m_running) {
        m_elapsed += (MPI_Wtime() - m_start_time);
        m_running = false;
    }
}

double Timer::elapsedSeconds() const {
    if (m_running) {
        return m_elapsed + (MPI_Wtime() - m_start_time);
    }
    return m_elapsed;
}

void Timer::reduceSummary(double local_time, MPI_Comm comm, double& min_time, double& max_time,
                          double& avg_time) {
    int comm_size = 1;
    MPI_Comm_size(comm, &comm_size);

    MPI_Reduce(&local_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, comm);
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    double sum_time = 0.0;
    MPI_Reduce(&local_time, &sum_time, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
    avg_time = sum_time / static_cast<double>(comm_size);
}

} // namespace heat
