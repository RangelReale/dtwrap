cmake_minimum_required(VERSION 2.8)
project(dtwrap)

set(CMAKE_CXX_STANDARD 14)

enable_testing()

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(duktape)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)
