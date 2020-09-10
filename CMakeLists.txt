cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(terminal CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11
        CACHE STRING "C++ standard" FORCE)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(terminal INTERFACE IMPORTED)
set_property(TARGET terminal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR})


enable_testing()

add_subdirectory(examples)

add_executable(test_terminal test_terminal.cpp)
target_link_libraries(test_terminal terminal)
add_test(test_terminal ${PROJECT_BINARY_DIR}/test_terminal)
