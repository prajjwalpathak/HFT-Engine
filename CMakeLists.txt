cmake_minimum_required(VERSION 3.20)

project(HFTEngine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build modes
set(CMAKE_CXX_FLAGS_DEBUG
    "-O0 -g -DENABLE_VALIDATION"
)

set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -march=native -flto -DNDEBUG"
)

add_subdirectory(src)