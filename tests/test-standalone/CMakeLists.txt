cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(test-standalone CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17
        CACHE STRING "C++ standard" FORCE)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(cpp-terminal REQUIRED)

add_executable(colors colors.cpp)
target_link_libraries(colors cpp-terminal)
