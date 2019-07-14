cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

file(READ VERSION.txt VERSION_INFORMATION)
string(STRIP ${VERSION_INFORMATION} VERSION_INFORMATION)
project(predicated_tree VERSION ${VERSION_INFORMATION} LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/.conan/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_COMPILER /usr/bin/clang++-7)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions -fno-omit-frame-pointer -O3")

add_subdirectory(include)

add_subdirectory(benchmark)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
