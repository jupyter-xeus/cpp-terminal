cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(cpp-terminal 
        VERSION 1.0.0
        DESCRIPTION "Small header only C++ library for writing multiplatform terminal applications"
        HOMEPAGE_URL "https://github.com/jupyter-xeus/cpp-terminal"
        LANGUAGES CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17
        CACHE STRING "C++ standard" FORCE)
endif ()

option(CPPTERMINAL_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(CPPTERMINAL_ENABLE_INSTALL "Set to ON to enable install" ON)
option(CPPTERMINAL_ENABLE_TESING "Set to ON to enable testing" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR "include" CACHE STRING "install include directory")
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION lib/${PROJECT_NAME}/cmake
  PATH_VARS CPPTERMINAL_CMAKECONFIG_INCLUDE_DIR)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION lib/${PROJECT_NAME}/cmake )


if (CPPTERMINAL_ENABLE_TESING)
    enable_testing()
    add_subdirectory(tests)
endif()

if (CPPTERMINAL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(cpp-terminal)

if (CPPTERMINAL_ENABLE_INSTALL)
    install(TARGETS cpp-terminal
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/cpp-terminal
    )
endif()
