cmake_minimum_required(VERSION 3.16)
project(SprigDB_Tests C)

set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /usr/include
)

# Collect source files from src/
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

# Collect test files from tests/
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# Create test executable
add_executable(run_tests ${SRC_FILES} ${TEST_FILES})

# Link external libraries manually
target_link_libraries(run_tests PRIVATE argon2 crypto ssl criterion m)

enable_testing()
add_test(NAME AllTests COMMAND run_tests)
