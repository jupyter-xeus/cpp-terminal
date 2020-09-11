cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(cpp-terminal CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11
        CACHE STRING "C++ standard" FORCE)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(cpp-terminal INTERFACE)
target_include_directories(cpp-terminal INTERFACE ${PROJECT_SOURCE_DIR})
set_target_properties(cpp-terminal PROPERTIES
    PUBLIC_HEADER "cpp-terminal/terminal.h;cpp-terminal/terminal_base.h"
)

set(CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR "include" CACHE STRING "install include directory")
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION lib/${PROJECT_NAME}/cmake
  PATH_VARS CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION lib/${PROJECT_NAME}/cmake )


enable_testing()

add_subdirectory(examples)

add_executable(test_terminal test_terminal.cpp)
target_link_libraries(test_terminal cpp-terminal)
add_test(test_terminal ${PROJECT_BINARY_DIR}/test_terminal)


install(TARGETS cpp-terminal
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/cpp-terminal
)
