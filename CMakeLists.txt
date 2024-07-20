cmake_minimum_required(VERSION 3.11)

project(fenrir)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
    googletest 
    # 1.14.0
    URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)

FetchContent_MakeAvailable(googletest)

include_directories(include)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

file(GLOB SOURCES "src/*.cpp")

# set(CMAKE_CXX_CPPCHECK "cppcheck;language=c++;--enable=all;--suppress=missingIncludeSystem;")
add_executable(fenrir ${SOURCES})
set_target_properties(fenrir PROPERTIES
    CXX_CPPCHECK "cppcheck;language=c++;--enable=all;--suppress=missingIncludeSystem;"
)

enable_testing()

file(GLOB TEST_SOURCES "test/*.cpp")

include(GoogleTest)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} GTest::gtest_main)
    gtest_discover_tests(${TEST_NAME})
endforeach()
