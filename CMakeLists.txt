cmake_minimum_required(VERSION 3.10)
project(danndy8 CXX)
set (CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(danndy8 src/main.cpp
                       src/CHIP8.h
                       src/CHIP8.cpp)

target_link_libraries(danndy8 PRIVATE SDL2::SDL2 SDL2::SDL2main)