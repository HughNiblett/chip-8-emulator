cmake_minimum_required(VERSION 3.26)
project(chip8)

find_package(SDL3 REQUIRED)

add_executable(chip8
    src/chip8.cpp
    src/sdl_interface.cpp
)


target_include_directories(chip8
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)


target_link_libraries(chip8 SDL3::SDL3)