# Master Build File
cmake_minimum_required(VERSION 3.25)

project(
    ARQYONPicShop
    VERSION 1.0.0
    DESCRIPTION "ARQYON PicShop - Professional Image Editing Software by ARQYON"
    HOMEPAGE_URL "https://ARQYON.co"
    LANGUAGES CXX
)

# -------------------------------------------------------
# C++ Standard
# We use C++17 - modern, stable, widely supported
# -------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------
# Project Metadata
# Used in splash screen, about dialog, installer
# -------------------------------------------------------
set(SVPICSHOP_VERSION_MAJOR 1)
set(SVPICSHOP_VERSION_MINOR 0)
set(SVPICSHOP_VERSION_PATCH 0)
set(SVPICSHOP_VERSION_STRING "1.0.0")
set(SVPICSHOP_APP_NAME "ARQYON PicShop")
set(SVPICSHOP_COMPANY_NAME "ARQYON")
set(SVPICSHOP_WEBSITE "https://ARQYON.co")
set(SVPICSHOP_AUTHOR "Adil Hassan")
set(SVPICSHOP_EXE_NAME "SVPicShop")

# -------------------------------------------------------
# Build type default
# If not specified, default to Release
# -------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "=== ARQYON PicShop Build System ===")
message(STATUS "Version    : ${SVPICSHOP_VERSION_STRING}")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
message(STATUS "Author     : ${SVPICSHOP_AUTHOR}")
message(STATUS "Website    : ${SVPICSHOP_WEBSITE}")
message(STATUS "=======================================")

# -------------------------------------------------------
# Output Directories
# All binaries go into build/bin, libraries into build/lib
# -------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# -------------------------------------------------------
# Find Qt6
# Qt6 is our GUI framework - must be installed and findable
# -------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
    OpenGLWidgets
    Svg
    Xml
    Network
)

# Enable Qt's Automatic Tools
set(CMAKE_AUTOMOC ON)  # Auto Meta Object Compiler
set(CMAKE_AUTORCC ON)  # Auto Resource Compiler
set(CMAKE_AUTOUIC ON)  # Auto UI Compiler

message(STATUS "Qt6 Version : ${Qt6_VERSION}")

# -------------------------------------------------------
# Compiler Flags
# Different flags for Debug vs Release
# Note: These are GCC/Clang flags. On MSVC, CMake maps
# Debug/Release to /Od+/Zi and /O2+/DNDEBUG automatically,
# so the add_compile_options below only applies on GCC/Clang.
# -------------------------------------------------------
if(NOT MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Debug build - enabling all warnings.")
        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -g
            -O0
        )
    else()
        message(STATUS "Release build - enabling optimization.")
        add_compile_options(
            -O2
            -DNDEBUG
        )
    endif()
endif()

# -------------------------------------------------------
# Windows-specific settings
# -------------------------------------------------------
if(WIN32)
    message(STATUS "Platform : Windows")

    # Hide the console window in Release builds.
    # /SUBSYSTEM:WINDOWS is the MSVC equivalent of -mwindows.
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        if(MSVC)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
        else()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
        endif()
    endif()

    # Windows app icon and version info
    set(WINDOWS_RESOURCES ${CMAKE_SOURCE_DIR}/assets/windows/app.rc)
endif()

# -------------------------------------------------------
# Subdirectories
# Each folder has its own CMakeLists.txt and is built in order.
# 1. Core image engine (pure C++, no Qt dependency)
# 2. Qt6 GUI layer (depends on core)
# 3. Tests (only when SVPICSHOP_BUILD_TESTS=ON)
# -------------------------------------------------------
add_subdirectory(core)

option(SVPICSHOP_BUILD_TESTS 
add_subdirectory(ui)"Build test suite" OFF)
if(SVPICSHOP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Tests : ENABLED")
else()
    message(STATUS "Tests : DISABLED (use -DSVPICSHOP_BUILD_TESTS=ON to enable)")
endif()

# -------------------------------------------------------
# Main Executable
# Combines core + ui into SVPicShop.exe
# Note: This main.cpp will eventually be the Qt app entry
# point, NOT the engine test file (which lives in tests/).
# -------------------------------------------------------
add_executable(${SVPICSHOP_EXE_NAME}
    main.cpp
    ${WINDOWS_RESOURCES}
)

target_link_libraries(${SVPICSHOP_EXE_NAME}
    PRIVATE
        svpicshop_core
        svpicshop_ui
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::Svg
        Qt6::Xml
        Qt6::Network
)

# Pass version info into source code as compile definitions
target_compile_definitions(${SVPICSHOP_EXE_NAME}
    PRIVATE
        SVPICSHOP_VERSION_MAJOR=${SVPICSHOP_VERSION_MAJOR}
        SVPICSHOP_VERSION_MINOR=${SVPICSHOP_VERSION_MINOR}
        SVPICSHOP_VERSION_PATCH=${SVPICSHOP_VERSION_PATCH}
        SVPICSHOP_VERSION_STRING="${SVPICSHOP_VERSION_STRING}"
        SVPICSHOP_APP_NAME="${SVPICSHOP_APP_NAME}"
        SVPICSHOP_COMPANY_NAME="${SVPICSHOP_COMPANY_NAME}"
        SVPICSHOP_WEBSITE="${SVPICSHOP_WEBSITE}"
)

# -------------------------------------------------------
# Install Rules
# Where files go when you run: cmake --install .
# -------------------------------------------------------
install(TARGETS ${SVPICSHOP_EXE_NAME}
    RUNTIME DESTINATION bin
)
install(DIRECTORY assets/
    DESTINATION assets
)

# -------------------------------------------------------
# Summary
# -------------------------------------------------------
message(STATUS "========================================")
message(STATUS "CMake Configuration Complete")
message(STATUS "Run: cmake --build . --config Release")
message(STATUS "========================================")