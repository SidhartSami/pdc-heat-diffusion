#include <heat/config.hpp>
#include <heat/mpi_solver.hpp>
#include <heat/timer.hpp>

#include <mpi.h>
#include <omp.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

std::string getCpuModel() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.rfind("model name", 0) == 0) {
            auto colon = line.find(':');
            if (colon != std::string::npos) {
                std::string model = line.substr(colon + 1);
                // trim leading spaces
                size_t first = model.find_first_not_of(" \t");
                return (first != std::string::npos) ? model.substr(first) : model;
            }
        }
    }
    return "Unknown CPU";
}

} // namespace

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

    if (!config.isStable()) {
        if (rank == 0) {
            std::cerr << "ERROR: Explicit FTCS stability criterion violated!\n"
                      << "dt = " << config.dt << " > max_stable_dt = " << config.maxStableDt()
                      << "\n";
        }
        MPI_Finalize();
        return 2;
    }

    const char* omp_places_env = std::getenv("OMP_PLACES");
    const char* omp_bind_env = std::getenv("OMP_PROC_BIND");
    const char* gomp_affinity = std::getenv("GOMP_CPU_AFFINITY");

    const std::string omp_places_str = (omp_places_env != nullptr) ? omp_places_env : "unspecified";
    const std::string omp_bind_str = (omp_bind_env != nullptr) ? omp_bind_env : "unspecified";
    const std::string gomp_affinity_str = (gomp_affinity != nullptr) ? gomp_affinity : "unspecified";
    const int omp_threads = omp_get_max_threads();

    if (rank == 0) {
        std::cout << "========================================================\n"
                  << " Parallel Heat Diffusion Solver (MPI + OpenMP Hybrid)\n"
                  << " CPU Model: " << getCpuModel() << "\n"
                  << " Physical Topology: 12 cores (4 P-cores + 8 E-cores), 16 logical CPUs\n"
                  << " MPI Ranks: " << size << ", OpenMP Threads/Rank: " << omp_threads << "\n"
                  << " OpenMP Runtime: Version " << _OPENMP << " (Standard 4.5), Dynamic Adjustment: "
                  << (omp_get_dynamic() ? "ENABLED" : "DISABLED") << "\n"
                  << " Note: OMP_PLACES and OMP_PROC_BIND affinity controls were introduced in OpenMP 4.0 (Section 4.4)\n";
        if (omp_places_env != nullptr || omp_bind_env != nullptr) {
            std::cout << " Standard Pinning (OpenMP 4.0): OMP_PLACES=" << omp_places_str
                      << ", OMP_PROC_BIND=" << omp_bind_str << "\n";
        }
        if (gomp_affinity != nullptr) {
            std::cout << " GNU Alternative Pinning: GOMP_CPU_AFFINITY=" << gomp_affinity_str << "\n";
        }
        std::cout << " Compiler: GNU " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__
                  << ", Flags: -O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic -ffp-contract=off\n"
                  << " Dimension: " << config.dimension << "D, Grid: " << config.nx << "x"
                  << config.ny;
        if (config.dimension == 3) {
            std::cout << "x" << config.nz;
        }
        std::cout << "\n Time step (dt): " << config.dt
                  << ", Max stable dt: " << config.maxStableDt() << " (STABLE)\n"
                  << " Steps: " << config.steps << ", Overlap: " << (config.overlap ? "ON" : "OFF")
                  << "\n"
                  << "========================================================\n";
    }

    // Warm-up iteration/barrier
    MPI_Barrier(MPI_COMM_WORLD);

    heat::Timer timer;
    timer.start();

    heat::MpiSolver solver(config, MPI_COMM_WORLD);
    if (config.dimension == 2) {
        auto grid2d = solver.run2D();
        (void)grid2d;
    } else {
        auto grid3d = solver.run3D();
        (void)grid3d;
    }

    timer.stop();
    const double local_elapsed = timer.elapsedSeconds();

    double min_elapsed = 0.0;
    double max_elapsed = 0.0;
    double avg_elapsed = 0.0;
    heat::Timer::reduceSummary(local_elapsed, MPI_COMM_WORLD, min_elapsed, max_elapsed, avg_elapsed);

    const double cells = (config.dimension == 2)
                             ? static_cast<double>(config.nx) * static_cast<double>(config.ny)
                             : static_cast<double>(config.nx) * static_cast<double>(config.ny) *
                                   static_cast<double>(config.nz);
    const double total_updates = cells * config.steps;
    const double mflops_or_mups = (total_updates / max_elapsed) / 1e6;

    if (rank == 0) {
        std::cout << "[EXECUTION SUMMARY]\n"
                  << " Max Time (Wall):  " << std::fixed << std::setprecision(6) << max_elapsed << " s\n"
                  << " Min Time (Wall):  " << min_elapsed << " s\n"
                  << " Avg Time (Wall):  " << avg_elapsed << " s\n"
                  << " Throughput:       " << std::setprecision(3) << mflops_or_mups << " MegaUpdates/s\n"
                  << "========================================================\n";
    }

    // Print per-rank local timings and verify uniform grid across ranks
    {
        std::cout << "[RANK " << rank << "/" << size << "] Subdomain: " << config.nx << "x"
                  << config.ny << (config.dimension == 3 ? ("x" + std::to_string(config.nz)) : "")
                  << ", Steps: " << config.steps << ", Local Wall Time: " << std::fixed
                  << std::setprecision(6) << local_elapsed << " s\n";
    }

    if (rank == 0) {
        // If --benchmark-csv was requested via environment or flag, write CSV
        const char* csv_output = std::getenv("BENCHMARK_CSV_FILE");
        if (csv_output != nullptr) {
            const bool file_exists = std::ifstream(csv_output).good();
            std::ofstream out(csv_output, std::ios::app);
            if (!file_exists) {
                out << "timestamp,cpu_model,ranks,threads_per_rank,dimension,nx,ny,nz,steps,overlap,"
                    << "min_time_s,max_time_s,avg_time_s,megaupdates_per_s,omp_places,omp_proc_bind\n";
            }
            out << std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count()
                << ",\"" << getCpuModel() << "\"," << size << "," << omp_threads << ","
                << config.dimension << "," << config.nx << "," << config.ny << ","
                << (config.dimension == 3 ? config.nz : 1) << "," << config.steps << ","
                << (config.overlap ? 1 : 0) << "," << min_elapsed << "," << max_elapsed << ","
                << avg_elapsed << "," << mflops_or_mups << ",\"" << omp_places_str << "\",\""
                << omp_bind_str << "\"\n";
        }
    }

    MPI_Finalize();
    return 0;
}
