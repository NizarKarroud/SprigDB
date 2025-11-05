cmake_minimum_required(VERSION 3.16)
project(MyProject C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)
