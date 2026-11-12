#include <heat/config.hpp>

#include <gtest/gtest.h>

#include <fstream>

TEST(ConfigTest, DefaultStability2D) {
    heat::Config cfg;
    cfg.dimension = 2;
    cfg.nx = 64;
    cfg.ny = 64;
    cfg.alpha = 1.0;
    // dx = dy = 1/64 = 0.015625
    // max_dt = 1 / (2 * 1 * (4096 + 4096)) = 1 / 16384 ~ 6.1035e-5
    cfg.dt = 1.0e-5;
    EXPECT_TRUE(cfg.isStable());

    cfg.dt = 1.0e-3;
    EXPECT_FALSE(cfg.isStable());
}

TEST(ConfigTest, StabilityLimitCalculation3D) {
    heat::Config cfg;
    cfg.dimension = 3;
    cfg.nx = 100;
    cfg.ny = 100;
    cfg.nz = 100;
    cfg.alpha = 1.0;
    // dx = 0.01, dx^2 = 0.0001
    // max_dt = 1.0 / (2.0 * 1.0 * (10000 + 10000 + 10000)) = 1.0 / 60000 ~ 1.666e-5
    const double expected_max_dt = 1.0 / 60000.0;
    EXPECT_NEAR(cfg.maxStableDt(), expected_max_dt, 1e-9);

    cfg.dt = 1.6e-5;
    EXPECT_TRUE(cfg.isStable());
    cfg.dt = 1.7e-5;
    EXPECT_FALSE(cfg.isStable());
}

TEST(ConfigTest, ParseFromFile) {
    const std::string filename = "test_tmp.cfg";
    {
        std::ofstream out(filename);
        out << "# Heat simulation test config\n";
        out << "dimension = 3\n";
        out << "nx = 128\n";
        out << "ny = 128\n";
        out << "nz = 64\n";
        out << "alpha = 0.5\n";
        out << "dt = 0.00005\n";
        out << "steps = 250\n";
        out << "boundary = neumann\n";
        out << "overlap = true\n";
    }

    heat::Config cfg = heat::ConfigParser::loadFromFile(filename);
    EXPECT_EQ(cfg.dimension, 3);
    EXPECT_EQ(cfg.nx, 128);
    EXPECT_EQ(cfg.ny, 128);
    EXPECT_EQ(cfg.nz, 64);
    EXPECT_DOUBLE_EQ(cfg.alpha, 0.5);
    EXPECT_DOUBLE_EQ(cfg.dt, 0.00005);
    EXPECT_EQ(cfg.steps, 250);
    EXPECT_EQ(cfg.boundary, "neumann");
    EXPECT_TRUE(cfg.overlap);

    std::remove(filename.c_str());
}

TEST(ConfigTest, CommandLineOverridesAndAutoDt) {
    const char* argv[] = {"prog",         "--dim=3",      "--nx=64",      "--ny=64",
                          "--nz=64",      "--dt=auto",    "--overlap=false", "--bz=32",
                          "--by=16",      "--steps=7",    "--snapshot-freq=5"};
    heat::Config cfg = heat::ConfigParser::loadFromCommandLine(
        static_cast<int>(sizeof(argv) / sizeof(argv[0])), const_cast<char**>(argv));
    EXPECT_EQ(cfg.dimension, 3);
    EXPECT_FALSE(cfg.overlap);
    EXPECT_EQ(cfg.block_size_z, 32);
    EXPECT_EQ(cfg.block_size_y, 16);
    EXPECT_EQ(cfg.steps, 7);
    EXPECT_EQ(cfg.snapshot_freq, 5);
    EXPECT_NEAR(cfg.dt, 0.9 * cfg.maxStableDt(), 1e-18);
    EXPECT_TRUE(cfg.isStable());
}

TEST(ConfigTest, UnknownCommandLineOptionIsRejected) {
    const char* argv[] = {"prog", "--bogus=1"};
    EXPECT_THROW(heat::ConfigParser::loadFromCommandLine(2, const_cast<char**>(argv)),
                 std::runtime_error);
}
