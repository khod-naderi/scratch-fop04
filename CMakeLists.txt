cmake_minimum_required(VERSION 3.10)
project(Scratch VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files from src/ directory
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create executable
add_executable(scratch ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(scratch PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Platform-specific SDL2 configuration
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(scratch PRIVATE 
        SDL2::SDL2 
        SDL2::SDL2main
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

    target_include_directories(scratch PRIVATE 
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
    target_link_libraries(scratch PRIVATE 
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
endif()

# Copy assets to build directory
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    file(COPY ${CMAKE_SOURCE_DIR}/assets 
         DESTINATION ${CMAKE_BINARY_DIR})
endif()