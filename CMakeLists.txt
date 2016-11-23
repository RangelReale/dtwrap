cmake_minimum_required(VERSION 2.8)
project(dtwrap)

enable_testing()

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(duktape)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)
