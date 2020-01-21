cmake_minimum_required(VERSION 3.15)

project(PhysikaX)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/_build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(phi-scripts)

# Warning Settings
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
endif()

# 3rd Party
find_package(gtest)
find_package(D3D12)

# Root source directory
add_subdirectory(src)
