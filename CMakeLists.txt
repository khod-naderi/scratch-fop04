
cmake_minimum_required(VERSION 3.10)
project(Scratch)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    add_executable(scratch main.cpp)
    target_link_libraries(scratch PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    add_executable(scratch main.cpp)
    target_include_directories(scratch PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(scratch ${SDL2_LIBRARIES})
endif()