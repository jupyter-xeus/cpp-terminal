add_subdirectory(private)

set(CPP_TERMINAL_PUBLIC_HEADERS
    args.hpp
    buffer.hpp
    color.hpp
    cursor.hpp
    event.hpp
    exception.hpp
    focus.hpp
    input.hpp
    iostream_initializer.hpp
    iostream.hpp
    key.hpp
    mouse.hpp
    options.hpp
    prompt.hpp
    screen.hpp
    stream.hpp
    style.hpp
    terminal_impl.hpp
    terminal_initializer.hpp
    terminal.hpp
    terminfo.hpp
    tty.hpp
    version.hpp
    window.hpp)

set(CPP_TERMINAL_SRC
    terminal_impl.cpp
    terminal_initializer.cpp
    iostream_initializer.cpp
    mouse.cpp
    args.cpp
    focus.cpp
    buffer.cpp
    iostream.cpp
    stream.cpp
    prompt.cpp
    window.cpp
    terminal.cpp
    color.cpp
    key.cpp
    event.cpp
    screen.cpp
    options.cpp
    cursor.cpp
    style.cpp)

# create and configure library target
add_library(cpp-terminal "${CPP_TERMINAL_SRC}")
target_link_libraries(cpp-terminal PRIVATE Warnings::Warnings cpp-terminal::cpp-terminal-private)
target_compile_options(cpp-terminal PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(cpp-terminal PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
set_target_properties(cpp-terminal PROPERTIES PUBLIC_HEADER "${CPP_TERMINAL_PUBLIC_HEADERS}")

add_library(cpp-terminal::cpp-terminal ALIAS cpp-terminal)

# Install
if (CPPTERMINAL_ENABLE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/cpp-terminalConfig.cmake.in" "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" COMPATIBILITY AnyNewerVersion)
  install(TARGETS cpp-terminal cpp-terminal-private cpp-terminalWarnings EXPORT cpp-terminalTargets
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpp-terminal")
  install(EXPORT cpp-terminalTargets FILE cpp-terminalTargets.cmake NAMESPACE cpp-terminal:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  install(FILES "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
endif()
