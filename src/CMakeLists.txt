# src/CMakeLists.txt
# -----------------
# This is for building the main program (sprigdb)

# Recursively collect all .c files in src/
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# Create the main executable
add_executable(sprigdb ${SRC_FILES})

# Include header directories
target_include_directories(sprigdb PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Find required libraries
find_package(OpenSSL REQUIRED)
# Argon2: link manually since no CMake package usually exists
# (install libargon2-0-dev on Ubuntu/Debian)
target_link_libraries(sprigdb PRIVATE ${OPENSSL_LIBRARIES} argon2)

# Optional: set C standard
set_target_properties(sprigdb PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
)
