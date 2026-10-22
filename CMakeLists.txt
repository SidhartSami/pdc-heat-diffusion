cmake_minimum_required(VERSION 3.20)
project(HeatDiffusionParallel
    VERSION 1.0.0
    DESCRIPTION "Parallel Heat Diffusion Simulation on Large 2D/3D Grids (MPI + OpenMP)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(HEAT_BUILD_TESTS "Build GoogleTest test suites" ON)
option(HEAT_ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)
option(HEAT_ENABLE_WERROR "Treat compiler warnings as errors" ON)

# Compiler Warnings and Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(HEAT_ENABLE_WERROR)
        add_compile_options(-Werror)
    endif()

    if(HEAT_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Find MPI (Required)
find_package(MPI REQUIRED)

# Find OpenMP (Required)
find_package(OpenMP REQUIRED)

# Include directories
include_directories(include)

# Core library
add_library(heat_core
    src/config.cpp
    src/timer.cpp
    src/boundary.cpp
    src/solver.cpp
)

target_include_directories(heat_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(heat_core PUBLIC
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
)

# Main executable
add_executable(heat_solver
    src/main.cpp
)

target_link_libraries(heat_solver PRIVATE
    heat_core
)

# Testing with GoogleTest via FetchContent
if(HEAT_BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    # Prevent installing GoogleTest alongside our project
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
