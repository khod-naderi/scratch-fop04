cmake_minimum_required(VERSION 3.10)
project(Scratch VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files from src/ directory
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Platform-specific icon and executable setup
if(WIN32)
    # Windows: Add resource file for icon
    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/resources/app.rc")
    add_executable(scratch WIN32 ${SOURCES} ${HEADERS} ${APP_ICON_RESOURCE})

elseif(APPLE)
    # macOS: Create app bundle with icon
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(APP_ICON_MACOS "${CMAKE_SOURCE_DIR}/assets/icons/icon.icns")
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    add_executable(scratch MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${APP_ICON_MACOS})

    set_target_properties(scratch PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Scratch"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_EXECUTABLE_NAME "scratch"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/Info.plist.in"
    )

else()
    # Linux: Regular executable (icons handled by .desktop file)
    add_executable(scratch ${SOURCES} ${HEADERS})
endif()

# Include directories
target_include_directories(scratch PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Platform-specific SDL2 configuration
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)

    target_link_libraries(scratch PRIVATE 
        SDL2::SDL2 
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
    )
elseif(APPLE)
    # macOS configuration
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

    target_include_directories(scratch PRIVATE 
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )

    target_link_directories(scratch PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_IMAGE_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
    )

    target_link_libraries(scratch PRIVATE 
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )

    target_link_libraries(scratch PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreAudio"
        "-framework AudioToolbox"
    )
else()
    # Linux configuration
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

    target_include_directories(scratch PRIVATE 
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )

    target_link_directories(scratch PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_IMAGE_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
    )

    target_link_libraries(scratch PRIVATE 
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
endif()

# Copy assets to build directory
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    if(APPLE)
        # For macOS bundle, copy to Resources folder
        file(COPY ${CMAKE_SOURCE_DIR}/assets 
             DESTINATION ${CMAKE_BINARY_DIR}/scratch.app/Contents/Resources)
    else()
        file(COPY ${CMAKE_SOURCE_DIR}/assets 
             DESTINATION ${CMAKE_BINARY_DIR})
    endif()
endif()

# Linux: Install .desktop file and icon (optional, for system installation)
if(UNIX AND NOT APPLE)
    install(TARGETS scratch DESTINATION bin)
    install(FILES ${CMAKE_SOURCE_DIR}/assets/icons/icon.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME scratch.png)
    install(FILES ${CMAKE_SOURCE_DIR}/resources/scratch.desktop
            DESTINATION share/applications)
endif()