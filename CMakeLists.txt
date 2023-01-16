cmake_minimum_required(VERSION 3.22)

project(PhysikaX)
# Set C++ standard to 2017
# Ref: https://matgomes.com/setting-cpp-standard-cmake/
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/_output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(common-helpers)

# Warning Settings
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
endif()

# 3rd Party
find_package(gtest)
find_package(glfw)
find_package(D3D12)
find_package(D3D11)

# Root source directory
add_subdirectory(src)
