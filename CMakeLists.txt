cmake_minimum_required(VERSION 3.10)

project(chip8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/external/olcPixelGameEngine)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
)

file(COPY ${PROJECT_SOURCE_DIR}/src/roms DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)