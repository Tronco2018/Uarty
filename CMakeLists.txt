cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Uarty VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES CMAKE_CONFIGURE_DEPENDS src/*.cpp)

add_executable(Uarty ${SOURCES})

target_include_directories(Uarty PRIVATE include)