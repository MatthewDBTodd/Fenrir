cmake_minimum_required(VERSION 3.11)

project(fenrir)

# set(CMAKE_VERBOSE_MAKEFILE ON)

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

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -fsanitize=address,undefined")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

file(GLOB LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(fenrir_lib ${LIB_SOURCES})

add_executable(fenrir "src/main.cpp")
target_link_libraries(fenrir fenrir_lib)

# set(CMAKE_CXX_CPPCHECK "cppcheck;language=c++;--enable=all;--suppress=missingIncludeSystem;")
# add_executable(fenrir ${SOURCES})
set_target_properties(fenrir PROPERTIES
    CXX_CPPCHECK "cppcheck;language=c++;--enable=all;--check-level=exhaustive;--inline-suppr;--suppress=missingIncludeSystem;--suppress=unusedFunction"
)
set_target_properties(fenrir_lib PROPERTIES
    CXX_CPPCHECK "cppcheck;language=c++;--enable=all;--check-level=exhaustive;--inline-suppr;--suppress=missingIncludeSystem;--suppress=unusedFunction"
)

enable_testing()
add_subdirectory(test)
