#include <heat/config.hpp>
#include <heat/solver.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: run_serial_bench <config_path>\n";
        return 1;
    }
    heat::Config config = heat::ConfigParser::loadFromFile(argv[1]);
    heat::SerialSolver solver(config);

    // Warm-up run
    solver.run2D();

    std::vector<double> times;
    for (int rep = 0; rep < 5; ++rep) {
        auto t0 = std::chrono::steady_clock::now();
        solver.run2D();
        auto t1 = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(t1 - t0).count();
        times.push_back(elapsed);
    }
    std::sort(times.begin(), times.end());
    double median = times[2];
    std::cout << "[SERIAL SOLVER BENCHMARK] Config: " << argv[1]
              << ", Median Time: " << median << " s"
              << ", Min: " << times.front() << " s"
              << ", Max: " << times.back() << " s\n";
    return 0;
}
