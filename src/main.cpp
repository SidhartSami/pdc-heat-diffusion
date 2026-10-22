#include <heat/config.hpp>
#include <heat/timer.hpp>

#include <mpi.h>
#include <omp.h>

#include <iostream>

int main(int argc, char** argv) {
    int provided = 0;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        std::cerr << "Error: MPI implementation does not provide MPI_THREAD_FUNNELED level support "
                     "(provided="
                  << provided << "). Aborting.\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    heat::Config config;
    try {
        config = heat::ConfigParser::loadFromCommandLine(argc, argv);
    } catch (const std::exception& e) {
        if (rank == 0) {
            std::cerr << "Configuration error: " << e.what() << "\n";
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        std::cout << "========================================================\n"
                  << " Parallel Heat Diffusion Solver (MPI + OpenMP)\n"
                  << " MPI Ranks: " << size << ", OpenMP Max Threads: " << omp_get_max_threads()
                  << "\n"
                  << " Dimension: " << config.dimension << "D, Grid: " << config.nx << "x"
                  << config.ny;
        if (config.dimension == 3) {
            std::cout << "x" << config.nz;
        }
        std::cout << "\n Time step (dt): " << config.dt
                  << ", Max stable dt: " << config.maxStableDt() << " ("
                  << (config.isStable() ? "STABLE" : "UNSTABLE") << ")\n"
                  << " Steps: " << config.steps << ", Overlap: " << (config.overlap ? "ON" : "OFF")
                  << "\n"
                  << "========================================================\n";
    }

    if (!config.isStable()) {
        if (rank == 0) {
            std::cerr << "ERROR: Explicit FTCS stability criterion violated!\n"
                      << "dt = " << config.dt << " > max_stable_dt = " << config.maxStableDt()
                      << "\n";
        }
        MPI_Finalize();
        return 2;
    }

    MPI_Finalize();
    return 0;
}
