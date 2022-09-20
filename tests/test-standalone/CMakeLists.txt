cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(test-standalone CXX)

find_package(cpp-terminal REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

get_filename_component(buildDirRelFilePath "${CMAKE_CURRENT_SOURCE_DIR}/../../examples/colors.cpp" REALPATH BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(colors "${buildDirRelFilePath}")
target_link_libraries(colors PRIVATE cpp-terminal::cpp-terminal)
