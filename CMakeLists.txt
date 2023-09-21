cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

project(cpp-terminal
        VERSION 1.0.0
        DESCRIPTION "Small C++ library for writing multiplatform terminal applications"
        HOMEPAGE_URL "https://github.com/jupyter-xeus/cpp-terminal"
        LANGUAGES CXX)

# see https://cmake.org/cmake/help/latest/policy/CMP0092.html
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_OLD "${CMAKE_CXX_FLAGS}")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  if(CMAKE_CXX_FLAGS MATCHES "/EH[ascr-]+")
    string(REGEX REPLACE "/EH[ascr-]+" "/EHsr" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

include(Warnings)

# set the C++ standard, if not set by the top level project
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard" FORCE)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED TRUE CACHE STRING "C++ standard required" FORCE)
set(CMAKE_CXX_EXTENSIONS FALSE CACHE STRING "C++ extensions" FORCE)

option(CPPTERMINAL_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(CPPTERMINAL_ENABLE_INSTALL "Set to ON to enable install" ON)
option(CPPTERMINAL_ENABLE_TESTING "Set to ON to enable testing" ON)
option(CPPTERMINAL_ENABLE_DOCS "Set to ON to generate documentation" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(cpp-terminal)

if(CPPTERMINAL_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

if(CPPTERMINAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(CPPTERMINAL_ENABLE_DOCS)
  add_subdirectory(docs)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_OLD})
endif()
