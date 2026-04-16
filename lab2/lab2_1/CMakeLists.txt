cmake_minimum_required(VERSION 3.10)

project(lab2_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(lab2_1 lab2_1.cpp)

target_link_libraries(lab2_1 PRIVATE OpenMP::OpenMP_CXX)