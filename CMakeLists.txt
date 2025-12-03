cmake_minimum_required(VERSION 3.20)
project(img-store VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(Threads REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Check for system-installed Crow
find_path(CROW_INCLUDE_DIR crow_all.h PATHS /usr/include /usr/local/include)
if(NOT CROW_INCLUDE_DIR)
    message(STATUS "Crow not found in system, will use header-only from lib/")
    include_directories(${PROJECT_SOURCE_DIR}/lib)
else()
    message(STATUS "Using system Crow from: ${CROW_INCLUDE_DIR}")
    include_directories(${CROW_INCLUDE_DIR})
endif()

# Check for system-installed xxHash
find_path(XXHASH_INCLUDE_DIR xxhash.h PATHS /usr/include /usr/local/include)
find_library(XXHASH_LIBRARY NAMES xxhash libxxhash)

if(NOT XXHASH_INCLUDE_DIR OR NOT XXHASH_LIBRARY)
    message(FATAL_ERROR "xxHash library not found. Please install xxHash development package.")
else()
    message(STATUS "Using system xxHash from: ${XXHASH_INCLUDE_DIR}")
    include_directories(${XXHASH_INCLUDE_DIR})
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/server.cpp
    src/image_handler.cpp
    src/storage_manager.cpp
    src/hash_utils.cpp
    src/auth_middleware.cpp
)

# Create executable
add_executable(img-store ${SOURCES})

# Link libraries
target_link_libraries(img-store
    PRIVATE
    Threads::Threads
    ${XXHASH_LIBRARY}
)

# Installation rules
install(TARGETS img-store DESTINATION bin)

# Print build configuration
message(STATUS "")
message(STATUS "Build Configuration:")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
