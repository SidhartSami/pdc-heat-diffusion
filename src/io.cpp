#include <heat/io.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace heat {

namespace fs = std::filesystem;

namespace {

constexpr char kCheckpointMagic[8] = {'H', 'E', 'A', 'T', 'C', 'K', 'P', '1'};

struct CheckpointHeader {
    char magic[8];
    std::int32_t dimension;
    std::int32_t nx, ny, nz;       ///< global size
    std::int32_t nranks;
    std::int32_t lnx, lny, lnz;    ///< local interior size
    std::int32_t step;
    double t;
};

std::string zeroPad(int value, int width = 6) {
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << value;
    return oss.str();
}

void ensureDirectory(const std::string& dir, int rank, MPI_Comm comm) {
    if (rank == 0 && !dir.empty()) {
        fs::create_directories(dir);
    }
    MPI_Barrier(comm);
}

std::string checkpointFile(const Config& config, int rank) {
    return config.checkpoint_path + "_r" + std::to_string(rank) + ".bin";
}

void writeCheckpointImpl(const Config& config, MPI_Comm comm, int rank, int nranks, int lnx,
                         int lny, int lnz, const std::vector<double>& interior, int step, double t) {
    const fs::path parent = fs::path(config.checkpoint_path).parent_path();
    ensureDirectory(parent.string(), rank, comm);

    CheckpointHeader header{};
    std::copy(std::begin(kCheckpointMagic), std::end(kCheckpointMagic), header.magic);
    header.dimension = config.dimension;
    header.nx = config.nx;
    header.ny = config.ny;
    header.nz = (config.dimension == 3) ? config.nz : 1;
    header.nranks = nranks;
    header.lnx = lnx;
    header.lny = lny;
    header.lnz = lnz;
    header.step = step;
    header.t = t;

    const std::string final_name = checkpointFile(config, rank);
    const std::string tmp_name = final_name + ".tmp";
    {
        std::ofstream out(tmp_name, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("Cannot open checkpoint file for writing: " + tmp_name);
        }
        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
        out.write(reinterpret_cast<const char*>(interior.data()),
                  static_cast<std::streamsize>(interior.size() * sizeof(double)));
        if (!out) {
            throw std::runtime_error("Failed writing checkpoint file: " + tmp_name);
        }
    }
    fs::rename(tmp_name, final_name);
    // No rank may race ahead (and possibly crash/overwrite) before every rank's file is in place.
    MPI_Barrier(comm);
}

void readCheckpointImpl(const Config& config, MPI_Comm comm, int rank, int nranks, int lnx, int lny,
                        int lnz, std::vector<double>& interior, int& step, double& t) {
    const std::string name = checkpointFile(config, rank);
    int local_ok = 1;
    std::string problem;
    CheckpointHeader header{};
    std::ifstream in(name, std::ios::binary);
    if (!in) {
        local_ok = 0;
        problem = "cannot open " + name;
    } else {
        in.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (!in || !std::equal(std::begin(kCheckpointMagic), std::end(kCheckpointMagic),
                               header.magic)) {
            local_ok = 0;
            problem = "bad header in " + name;
        } else if (header.dimension != config.dimension || header.nx != config.nx ||
                   header.ny != config.ny ||
                   header.nz != ((config.dimension == 3) ? config.nz : 1) ||
                   header.nranks != nranks || header.lnx != lnx || header.lny != lny ||
                   header.lnz != lnz) {
            local_ok = 0;
            problem = "checkpoint " + name +
                      " does not match the current grid / rank count (restart requires the same "
                      "decomposition)";
        } else {
            interior.resize(static_cast<std::size_t>(lnx) * static_cast<std::size_t>(lny) *
                            static_cast<std::size_t>(lnz));
            in.read(reinterpret_cast<char*>(interior.data()),
                    static_cast<std::streamsize>(interior.size() * sizeof(double)));
            if (!in) {
                local_ok = 0;
                problem = "truncated data in " + name;
            }
        }
    }

    // Collective agreement: every rank must have a valid file at the same step.
    int all_ok = 0;
    MPI_Allreduce(&local_ok, &all_ok, 1, MPI_INT, MPI_MIN, comm);
    int step_min = 0;
    int step_max = 0;
    const int my_step = local_ok != 0 ? header.step : 0;
    MPI_Allreduce(&my_step, &step_min, 1, MPI_INT, MPI_MIN, comm);
    MPI_Allreduce(&my_step, &step_max, 1, MPI_INT, MPI_MAX, comm);
    if (all_ok == 0 || step_min != step_max) {
        throw std::runtime_error(
            "Restart failed on rank " + std::to_string(rank) + ": " +
            (problem.empty() ? "ranks disagree on the checkpoint step" : problem));
    }
    step = header.step;
    t = header.t;
}

} // namespace

// ---------------------------------------------------------------------------------------------
// Checkpoint API
// ---------------------------------------------------------------------------------------------

void writeCheckpoint2D(const Config& config, const Decomposition2D& decomp, const Grid2D& grid,
                       int step, double t) {
    std::vector<double> buf;
    buf.reserve(static_cast<std::size_t>(decomp.localNx()) * static_cast<std::size_t>(decomp.localNy()));
    for (int j = 1; j <= decomp.localNy(); ++j) {
        for (int i = 1; i <= decomp.localNx(); ++i) {
            buf.push_back(grid(i, j));
        }
    }
    writeCheckpointImpl(config, decomp.cartComm(), decomp.rank(), decomp.size(), decomp.localNx(),
                        decomp.localNy(), 1, buf, step, t);
}

void writeCheckpoint3D(const Config& config, const Decomposition3D& decomp, const Grid3D& grid,
                       int step, double t) {
    std::vector<double> buf;
    buf.reserve(static_cast<std::size_t>(decomp.localNx()) * static_cast<std::size_t>(decomp.localNy()) *
                static_cast<std::size_t>(decomp.localNz()));
    for (int k = 1; k <= decomp.localNz(); ++k) {
        for (int j = 1; j <= decomp.localNy(); ++j) {
            for (int i = 1; i <= decomp.localNx(); ++i) {
                buf.push_back(grid(i, j, k));
            }
        }
    }
    writeCheckpointImpl(config, decomp.cartComm(), decomp.rank(), decomp.size(), decomp.localNx(),
                        decomp.localNy(), decomp.localNz(), buf, step, t);
}

void readCheckpoint2D(const Config& config, const Decomposition2D& decomp, Grid2D& grid, int& step,
                      double& t) {
    std::vector<double> buf;
    readCheckpointImpl(config, decomp.cartComm(), decomp.rank(), decomp.size(), decomp.localNx(),
                       decomp.localNy(), 1, buf, step, t);
    std::size_t idx = 0;
    for (int j = 1; j <= decomp.localNy(); ++j) {
        for (int i = 1; i <= decomp.localNx(); ++i) {
            grid(i, j) = buf[idx++];
        }
    }
}

void readCheckpoint3D(const Config& config, const Decomposition3D& decomp, Grid3D& grid, int& step,
                      double& t) {
    std::vector<double> buf;
    readCheckpointImpl(config, decomp.cartComm(), decomp.rank(), decomp.size(), decomp.localNx(),
                       decomp.localNy(), decomp.localNz(), buf, step, t);
    std::size_t idx = 0;
    for (int k = 1; k <= decomp.localNz(); ++k) {
        for (int j = 1; j <= decomp.localNy(); ++j) {
            for (int i = 1; i <= decomp.localNx(); ++i) {
                grid(i, j, k) = buf[idx++];
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------
// VTK writer
// ---------------------------------------------------------------------------------------------

VtkWriter::VtkWriter(const Config& config, MPI_Comm comm)
    : m_config(config)
    , m_comm(comm)
    , m_dir(config.output_path) {
    MPI_Comm_rank(comm, &m_rank);
    ensureDirectory(m_dir, m_rank, comm);
}

std::string VtkWriter::stem(int step) const {
    return "heat_" + zeroPad(step);
}

void VtkWriter::writePiece(const std::string& file, const std::array<int, 6>& e,
                           const std::vector<double>& cells) const {
    const double dx = 1.0 / m_config.nx;
    const double dy = 1.0 / m_config.ny;
    const double dz = (m_config.dimension == 3) ? 1.0 / m_config.nz : 1.0;
    const int whole_z = (m_config.dimension == 3) ? m_config.nz : 0;

    std::ofstream out(file, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Cannot open snapshot file for writing: " + file);
    }
    out << "<?xml version=\"1.0\"?>\n"
        << "<VTKFile type=\"ImageData\" version=\"1.0\" byte_order=\"LittleEndian\" "
           "header_type=\"UInt32\">\n"
        << "  <ImageData WholeExtent=\"0 " << m_config.nx << " 0 " << m_config.ny << " 0 "
        << whole_z << "\" Origin=\"0 0 0\" Spacing=\"" << dx << " " << dy << " " << dz << "\">\n"
        << "    <Piece Extent=\"" << e[0] << " " << e[1] << " " << e[2] << " " << e[3] << " "
        << e[4] << " " << e[5] << "\">\n"
        << "      <CellData Scalars=\"temperature\">\n"
        << "        <DataArray type=\"Float64\" Name=\"temperature\" format=\"appended\" "
           "offset=\"0\"/>\n"
        << "      </CellData>\n"
        << "    </Piece>\n"
        << "  </ImageData>\n"
        << "  <AppendedData encoding=\"raw\">\n_";
    const std::uint32_t nbytes = static_cast<std::uint32_t>(cells.size() * sizeof(double));
    out.write(reinterpret_cast<const char*>(&nbytes), sizeof(nbytes));
    out.write(reinterpret_cast<const char*>(cells.data()), static_cast<std::streamsize>(nbytes));
    out << "\n  </AppendedData>\n</VTKFile>\n";
    if (!out) {
        throw std::runtime_error("Failed writing snapshot file: " + file);
    }
}

void VtkWriter::writeMaster(const std::string& master, const std::vector<int>& all_extents,
                            int nranks, int step) const {
    const double dx = 1.0 / m_config.nx;
    const double dy = 1.0 / m_config.ny;
    const double dz = (m_config.dimension == 3) ? 1.0 / m_config.nz : 1.0;
    const int whole_z = (m_config.dimension == 3) ? m_config.nz : 0;

    std::ofstream out(master, std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Cannot open snapshot master for writing: " + master);
    }
    out << "<?xml version=\"1.0\"?>\n"
        << "<VTKFile type=\"PImageData\" version=\"1.0\" byte_order=\"LittleEndian\" "
           "header_type=\"UInt32\">\n"
        << "  <PImageData WholeExtent=\"0 " << m_config.nx << " 0 " << m_config.ny << " 0 "
        << whole_z << "\" GhostLevel=\"0\" Origin=\"0 0 0\" Spacing=\"" << dx << " " << dy << " "
        << dz << "\">\n"
        << "    <PCellData Scalars=\"temperature\">\n"
        << "      <PDataArray type=\"Float64\" Name=\"temperature\"/>\n"
        << "    </PCellData>\n";
    for (int r = 0; r < nranks; ++r) {
        const int* e = &all_extents[static_cast<std::size_t>(r) * 6];
        out << "    <Piece Extent=\"" << e[0] << " " << e[1] << " " << e[2] << " " << e[3] << " "
            << e[4] << " " << e[5] << "\" Source=\"" << stem(step) << "_r" << r << ".vti\"/>\n";
    }
    out << "  </PImageData>\n</VTKFile>\n";
}

void VtkWriter::writePvd() const {
    std::ofstream out(m_dir + "/heat.pvd", std::ios::trunc);
    out << "<?xml version=\"1.0\"?>\n"
        << "<VTKFile type=\"Collection\" version=\"1.0\" byte_order=\"LittleEndian\">\n"
        << "  <Collection>\n";
    for (const auto& [time, name] : m_entries) {
        out << "    <DataSet timestep=\"" << std::setprecision(12) << time
            << "\" group=\"\" part=\"0\" file=\"" << name << "\"/>\n";
    }
    out << "  </Collection>\n</VTKFile>\n";
}

void VtkWriter::write2D(const Decomposition2D& decomp, const Grid2D& grid, int step, double t) {
    std::vector<double> cells;
    cells.reserve(static_cast<std::size_t>(decomp.localNx()) * static_cast<std::size_t>(decomp.localNy()));
    for (int j = 1; j <= decomp.localNy(); ++j) {
        for (int i = 1; i <= decomp.localNx(); ++i) {
            cells.push_back(grid(i, j));
        }
    }
    const std::array<int, 6> extent{decomp.startX(), decomp.startX() + decomp.localNx(),
                                    decomp.startY(), decomp.startY() + decomp.localNy(), 0, 0};
    writePiece(m_dir + "/" + stem(step) + "_r" + std::to_string(decomp.rank()) + ".vti", extent,
               cells);

    std::vector<int> all(static_cast<std::size_t>(decomp.size()) * 6);
    MPI_Gather(extent.data(), 6, MPI_INT, all.data(), 6, MPI_INT, 0, decomp.cartComm());
    if (decomp.rank() == 0) {
        writeMaster(m_dir + "/" + stem(step) + ".pvti", all, decomp.size(), step);
        m_entries.emplace_back(t, stem(step) + ".pvti");
        writePvd();
    }
    MPI_Barrier(decomp.cartComm());
}

void VtkWriter::write3D(const Decomposition3D& decomp, const Grid3D& grid, int step, double t) {
    std::vector<double> cells;
    cells.reserve(static_cast<std::size_t>(decomp.localNx()) * static_cast<std::size_t>(decomp.localNy()) *
                  static_cast<std::size_t>(decomp.localNz()));
    for (int k = 1; k <= decomp.localNz(); ++k) {
        for (int j = 1; j <= decomp.localNy(); ++j) {
            for (int i = 1; i <= decomp.localNx(); ++i) {
                cells.push_back(grid(i, j, k));
            }
        }
    }
    const std::array<int, 6> extent{decomp.startX(), decomp.startX() + decomp.localNx(),
                                    decomp.startY(), decomp.startY() + decomp.localNy(),
                                    decomp.startZ(), decomp.startZ() + decomp.localNz()};
    writePiece(m_dir + "/" + stem(step) + "_r" + std::to_string(decomp.rank()) + ".vti", extent,
               cells);

    std::vector<int> all(static_cast<std::size_t>(decomp.size()) * 6);
    MPI_Gather(extent.data(), 6, MPI_INT, all.data(), 6, MPI_INT, 0, decomp.cartComm());
    if (decomp.rank() == 0) {
        writeMaster(m_dir + "/" + stem(step) + ".pvti", all, decomp.size(), step);
        m_entries.emplace_back(t, stem(step) + ".pvti");
        writePvd();
    }
    MPI_Barrier(decomp.cartComm());
}

} // namespace heat
