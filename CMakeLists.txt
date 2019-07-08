cmake_minimum_required(VERSION 3.10)

set(TREE_VERSION_MAJOR 0)
set(TREE_VERSION_MINOR 1)
set(TREE_VERSION_PATCH 0)

set(CMAKE_CXX_COMPILER /usr/bin/clang++-5.0)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -O3")

include_directories("${PROJECT_BINARY_DIR}/include")

add_library(tree INTERFACE)
target_include_directories(tree INTERFACE include/)

enable_testing()
add_subdirectory(test)
