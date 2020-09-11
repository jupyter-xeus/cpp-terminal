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

set(CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE STRING "install include directory")
set(CPPTERMINAL_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/cmake" CACHE STRING "install path for cpp-terminalConfig.cmake")
include(CMakePackageConfigHelpers)
configure_package_config_file(cpp-terminalConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfig.cmake
  INSTALL_DESTINATION lib/cpp-terminal/cmake
  PATH_VARS CPPTERMINAL_CMAKECONFIG_INSTALL_DIR
  CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfig.cmake
        DESTINATION lib/cpp-terminal/cmake )


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
