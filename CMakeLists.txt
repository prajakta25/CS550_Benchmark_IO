cmake_minimum_required(VERSION 3.10)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()
set (CMAKE_CXX_FLAGS "-std=c++2a ${CMAKE_CXX_FLAGS}")
project(benchmark_proj)

# Create test executable

add_executable(gen workload_generator.cpp)

# Enable tests
option(BUILD_TEST "Build the unit tests" OFF)
if(BUILD_TEST)
    enable_testing()
    include(CTest)
endif()

# Create basic test
add_test(NAME gentest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gen -client Posix -fn hello.txt -bs 1m -rcnt 1 -wcnt 1 -stat 24)
