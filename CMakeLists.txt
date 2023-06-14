cmake_minimum_required(VERSION 3.0)
project(InductiveLogicProgramming)

# Add executable for sequential
add_executable(sequential src/sequential.cpp src/utils.cpp)

# Add executable for OpenMP version
find_package(OpenMP REQUIRED)
add_executable(openmp src/openmp.cpp src/utils.cpp)
target_link_libraries(openmp PRIVATE OpenMP::OpenMP_CXX)