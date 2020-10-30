cmake_minimum_required(VERSION 3.10)
project(benchmark_proj)

# Create test executable
SET(CMAKE_EXE_LINK_FLAGS "-pthread -lrt")
SET(UMAP_LIB "/home/prajakta/CS550_Benchmark_IO/lib/libumap.so")
SET(UMMAP_LIB "/home/prajakta/CS550_Benchmark_IO/lib/libumalloc.a")
add_definitions(${CMAKE_EXE_LINK_FLAGS})
add_executable(generator workload_generator.cpp)
target_link_libraries(generator ${CMAKE_EXE_LINK_FLAGS})
target_link_libraries(generator ${UMAP_LIB})
target_link_libraries(generator ${UMMAP_LIB})

# Enable tests
option(BUILD_TEST "Build the unit tests" OFF)
if(BUILD_TEST)
    enable_testing()
    include(CTest)
endif()

# Create basic test
add_test(NAME gentest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/generator -client Posix -fn hello.txt -bs 1m -rcnt 1 -wcnt 1 -stat 24)
