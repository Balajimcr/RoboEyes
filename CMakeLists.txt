cmake_minimum_required(VERSION 3.15)

project(RoboEyes
    VERSION 1.0
    DESCRIPTION "Embedded-style Eye Expression Renderer using OpenCV"
    LANGUAGES CXX
)

# ----------------------------------
# MSVC / Windows Configuration
# ----------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable min/max macro pollution
add_compile_definitions(NOMINMAX)

# ----------------------------------
# OpenCV
# ----------------------------------
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# ----------------------------------
# Executable
# ----------------------------------
add_executable(roboeyes
    source/RoboEyes.cpp
)

# ----------------------------------
# Includes
# ----------------------------------
target_include_directories(roboeyes
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

# ----------------------------------
# Link
# ----------------------------------
target_link_libraries(roboeyes
    PRIVATE
        ${OpenCV_LIBS}
)

# ----------------------------------
# MSVC Warnings (Safe Set)
# ----------------------------------
if (MSVC)
    target_compile_options(roboeyes PRIVATE /W4)
endif()

# ----------------------------------
# Runtime Working Directory
# (So PNG loads correctly)
# ----------------------------------
set_target_properties(roboeyes PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}"
)
