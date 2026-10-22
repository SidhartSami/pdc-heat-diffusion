add_executable(test_config
    test_config.cpp
)

target_link_libraries(test_config PRIVATE
    heat_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_config)
