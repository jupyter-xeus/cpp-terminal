cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(terminal CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11
        CACHE STRING "C++ standard" FORCE)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(mylib INTERFACE)

add_library(terminal INTERFACE)
target_include_directories(terminal INTERFACE ${PROJECT_SOURCE_DIR})
set_target_properties(terminal PROPERTIES
    PUBLIC_HEADER "terminal.h;terminal_base.h"
)


enable_testing()

add_subdirectory(examples)

add_executable(test_terminal test_terminal.cpp)
target_link_libraries(test_terminal terminal)
add_test(test_terminal ${PROJECT_BINARY_DIR}/test_terminal)


install(TARGETS terminal
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
