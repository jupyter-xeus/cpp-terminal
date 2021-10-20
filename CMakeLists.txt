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
configure_file(cpp-terminal/version.h.in cpp-terminal/version.h)

option(CPPTERMINAL_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(CPPTERMINAL_ENABLE_INSTALL "Set to ON to enable install" ON)
option(CPPTERMINAL_ENABLE_TESING "Set to ON to enable testing" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(cpp-terminal cpp-terminal/terminal_base.cpp)
target_include_directories(cpp-terminal BEFORE PUBLIC
    ${cpp-terminal_SOURCE_DIR})
target_include_directories(cpp-terminal INTERFACE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
set_target_properties(cpp-terminal PROPERTIES
    PUBLIC_HEADER "cpp-terminal/terminal.h;cpp-terminal/terminal_base.h;cpp-terminal/version.h"
)

if (CPPTERMINAL_ENABLE_TESING)
    enable_testing()
    add_subdirectory(tests)
endif()

if (CPPTERMINAL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CPPTERMINAL_ENABLE_INSTALL)
    install(TARGETS cpp-terminal EXPORT cpp-terminalTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/cpp-terminal
    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalConfigVersion.cmake"
      VERSION ${CPPTERMINAL_VERSION}
      COMPATIBILITY AnyNewerVersion
    )

    export(EXPORT cpp-terminalTargets
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalTargets.cmake"
    )
    
    set(ConfigPackageLocation lib/cmake/cpp-terminal)

    configure_file(cpp-terminalConfig.cmake.in
      "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalConfig.cmake"
    )

    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminal/cpp-terminalTargets.cmake"
      DESTINATION
        ${ConfigPackageLocation}
      COMPONENT
        Devel
    )

endif()
