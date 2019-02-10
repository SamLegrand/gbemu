cmake_minimum_required(VERSION 3.12)
project(GameboyEmulator)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

find_package(SFML 2 COMPONENTS system window graphics)

include_directories(${SFML_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(GameboyEmulator main.cpp CPU.cpp CPU.h MMU.cpp MMU.h GPU.cpp GPU.h)

target_link_libraries(GameboyEmulator ${SFML_LIBRARIES})