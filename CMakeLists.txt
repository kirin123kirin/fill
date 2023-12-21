cmake_minimum_required(VERSION 3.15)

project(fill)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(
            fill
            fill.cpp
        )

