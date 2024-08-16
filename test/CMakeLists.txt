file(GLOB TEST_SOURCES "*.cpp")

include(GoogleTest)

add_executable(test_fenrir ${TEST_SOURCES})
target_link_libraries(test_fenrir PRIVATE GTest::gtest_main fenrir_lib Boost::stacktrace_basic dl backtrace)
gtest_discover_tests(test_fenrir DISCOVERY_TIMEOUT 600)

add_custom_target(
    run_tests
    COMMAND test_fenrir
    DEPENDS test_fenrir
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
