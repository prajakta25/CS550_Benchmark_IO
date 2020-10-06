cmake_minimum_required(VERSION 3.10)
project(benchmark_proj)

# Create test executable
SET(CMAKE_EXE_LINK_FLAGS "-lrt")
add_definitions(${CMAKE_EXE_LINK_FLAGS})
add_executable(gen workload_generator.cpp)
target_link_libraries(gen ${CMAKE_EXE_LINK_FLAGS})

# Enable tests
option(BUILD_TEST "Build the unit tests" OFF)
if(BUILD_TEST)
    enable_testing()
    include(CTest)
endif()

# Create basic test
add_test(NAME gentest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gen -client Posix -fn hello.txt -bs 1m -rcnt 1 -wcnt 1 -stat 24)
