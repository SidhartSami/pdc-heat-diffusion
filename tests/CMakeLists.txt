add_executable(test_config
    test_config.cpp
)

target_link_libraries(test_config PRIVATE
    heat_core
    GTest::gtest_main
)

add_executable(test_serial_solver
    test_serial_solver.cpp
)

target_link_libraries(test_serial_solver PRIVATE
    heat_core
    GTest::gtest_main
)

add_executable(test_mpi
    test_mpi.cpp
)

target_link_libraries(test_mpi PRIVATE
    heat_core
    GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(test_config)
gtest_discover_tests(test_serial_solver)
gtest_discover_tests(test_mpi DISCOVERY_MODE PRE_TEST)
