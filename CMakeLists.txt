cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(cpp-terminal 
        VERSION 1.0.0
        DESCRIPTION "Small header only C++ library for writing multiplatform terminal applications"
        HOMEPAGE_URL "https://github.com/jupyter-xeus/cpp-terminal"
        LANGUAGES CXX)

# set the C++ standard, if not set by the top level project
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17
        CACHE STRING "C++ standard" FORCE)
endif ()

option(CPPTERMINAL_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(CPPTERMINAL_ENABLE_INSTALL "Set to ON to enable install" ON)
option(CPPTERMINAL_ENABLE_TESING "Set to ON to enable testing" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(cpp-terminal)

if (CPPTERMINAL_ENABLE_TESING)
    enable_testing()
    add_subdirectory(tests)
endif()

if (CPPTERMINAL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CPPTERMINAL_ENABLE_INSTALL)
  include(cmake/install.cmake)
  install_library(cpp-terminal)

endif()
