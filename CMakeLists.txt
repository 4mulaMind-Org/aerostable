cmake_minimum_required(VERSION 3.28)
project(aerostable LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(control_core STATIC)
target_sources(control_core PUBLIC FILE_SET CXX_MODULES FILES src/core.cppm)

add_library(control_render STATIC)
target_sources(control_render PUBLIC FILE_SET CXX_MODULES FILES src/render.cppm)
target_link_libraries(control_render PUBLIC control_core)

# Naya Simulation Module
add_library(control_simulation STATIC)
target_sources(control_simulation PUBLIC FILE_SET CXX_MODULES FILES src/simulation.cppm)
target_link_libraries(control_simulation PUBLIC control_core)

add_executable(aerostable src/main.cpp)
target_link_libraries(aerostable PRIVATE control_core control_render control_simulation)

foreach(tgt control_core control_render control_simulation aerostable)
    target_compile_options(${tgt} PRIVATE -Wall -Wextra -O2)
endforeach()