cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 17)

# Find SDL2 via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS} src include)
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(chip8 src/main.cpp src/Chip8.cpp src/DisplayRenderer.cpp)
target_link_libraries(chip8 ${SDL2_LIBRARIES})

