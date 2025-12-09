cmake_minimum_required(VERSION 3.26)
project(chip8)

find_package(SDL3 REQUIRED)

add_executable(chip8)

target_sources(chip8
PRIVATE
    test.cpp
)

target_link_libraries(chip8 SDL3::SDL3)