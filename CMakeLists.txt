cmake_minimum_required(VERSION 3.10)
project(Scratch)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(scratch main.cpp)
target_link_libraries(scratch SDL2::SDL2)