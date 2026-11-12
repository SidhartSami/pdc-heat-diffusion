#include <heat/config.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace heat {

static inline std::string trim(const std::string& str) {
    const auto first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool Config::isStable() const {
    return dt <= maxStableDt();
}

double Config::maxStableDt() const {
    const double dx = 1.0 / static_cast<double>(nx);
    const double dy = 1.0 / static_cast<double>(ny);
    double inv_d_sq = (1.0 / (dx * dx)) + (1.0 / (dy * dy));

    if (dimension == 3) {
        const double dz = 1.0 / static_cast<double>(nz);
        inv_d_sq += (1.0 / (dz * dz));
    }

    // Explicit FTCS stability: dt <= 1 / (2 * alpha * sum(1/dx_i^2))
    return 1.0 / (2.0 * alpha * inv_d_sq);
}

namespace {

bool parseBool(const std::string& val) {
    std::string lower = val;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

/// Applies a single key/value pair. Returns false if the key is unknown.
/// A dt of "auto" selects 0.9 x the FTCS stability limit (after grid sizes are known).
bool applyOption(Config& cfg, const std::string& key, const std::string& val) {
    if (key == "dimension" || key == "dim") {
        cfg.dimension = std::stoi(val);
    } else if (key == "nx") {
        cfg.nx = std::stoi(val);
    } else if (key == "ny") {
        cfg.ny = std::stoi(val);
    } else if (key == "nz") {
        cfg.nz = std::stoi(val);
    } else if (key == "alpha") {
        cfg.alpha = std::stod(val);
    } else if (key == "dt") {
        if (val == "auto") {
            cfg.dt_auto = true;
        } else {
            cfg.dt_auto = false;
            cfg.dt = std::stod(val);
        }
    } else if (key == "steps") {
        cfg.steps = std::stoi(val);
    } else if (key == "boundary") {
        cfg.boundary = val;
    } else if (key == "initial_condition" || key == "ic") {
        cfg.initial_condition = val;
    } else if (key == "overlap") {
        cfg.overlap = parseBool(val);
    } else if (key == "snapshot_freq") {
        cfg.snapshot_freq = std::stoi(val);
    } else if (key == "output_path" || key == "output") {
        cfg.output_path = val;
    } else if (key == "block_size_x") {
        cfg.block_size_x = std::stoi(val);
    } else if (key == "block_size_y") {
        cfg.block_size_y = std::stoi(val);
    } else if (key == "block_size_z") {
        cfg.block_size_z = std::stoi(val);
    } else if (key == "checkpoint_freq") {
        cfg.checkpoint_freq = std::stoi(val);
    } else if (key == "checkpoint_path") {
        cfg.checkpoint_path = val;
    } else if (key == "restart_from") {
        cfg.restart_from = val;
    } else {
        return false;
    }
    return true;
}

void finalizeConfig(Config& cfg) {
    if (cfg.dt_auto) {
        cfg.dt = 0.9 * cfg.maxStableDt();
    }
}

} // namespace

Config ConfigParser::loadFromFile(const std::string& filepath) {
    Config cfg;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filepath);
    }

    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        const auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        const auto eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            throw std::runtime_error("Malformed config syntax at line " +
                                     std::to_string(line_number) + ": " + line);
        }

        const std::string key = trim(line.substr(0, eq_pos));
        const std::string val = trim(line.substr(eq_pos + 1));
        if (!applyOption(cfg, key, val)) {
            std::cerr << "Warning: Unknown config key '" << key << "' at line " << line_number
                      << "\n";
        }
    }

    finalizeConfig(cfg);
    return cfg;
}

Config ConfigParser::loadFromCommandLine(int argc, char** argv, const Config& base) {
    Config cfg = base;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            cfg = loadFromFile(argv[++i]);
        } else if (arg == "--overlap") {
            cfg.overlap = true;
        } else if (arg.rfind("--", 0) == 0) {
            const auto eq = arg.find('=');
            if (eq == std::string::npos) {
                throw std::runtime_error("Unrecognised option: " + arg);
            }
            std::string key = arg.substr(2, eq - 2);
            std::replace(key.begin(), key.end(), '-', '_');
            const std::string val = arg.substr(eq + 1);
            if (key == "config") {
                cfg = loadFromFile(val);
            } else if (key == "bx") {
                cfg.block_size_x = std::stoi(val);
            } else if (key == "by") {
                cfg.block_size_y = std::stoi(val);
            } else if (key == "bz") {
                cfg.block_size_z = std::stoi(val);
            } else if (!applyOption(cfg, key, val)) {
                throw std::runtime_error("Unrecognised option: " + arg);
            }
        } else {
            throw std::runtime_error("Unrecognised argument: " + arg);
        }
    }
    finalizeConfig(cfg);
    return cfg;
}

} // namespace heat
