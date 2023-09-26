# configure version information
configure_file(version.cpp.in version.cpp)

add_subdirectory(platforms)

# create and configure library target
add_library(cpp-terminal mouse.cpp args.cpp focus.cpp buffer.cpp iostream.cpp stream.cpp prompt.cpp window.cpp terminal.cpp color.cpp key.cpp event.cpp screen.cpp options.cpp cursor.cpp style.cpp "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")
target_link_libraries(cpp-terminal PRIVATE Warnings::Warnings cpp-terminal::cpp-terminal-platforms)
target_compile_options(cpp-terminal PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(cpp-terminal PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)

set_target_properties(cpp-terminal PROPERTIES
                      PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/focus.hpp;${CMAKE_CURRENT_SOURCE_DIR}/options.hpp;${CMAKE_CURRENT_SOURCE_DIR}/event.hpp;${CMAKE_CURRENT_SOURCE_DIR}/key.hpp;${CMAKE_CURRENT_SOURCE_DIR}/tty.hpp;${CMAKE_CURRENT_SOURCE_DIR}/terminal.hpp;${CMAKE_CURRENT_SOURCE_DIR}/cursor.hpp;${CMAKE_CURRENT_SOURCE_DIR}/style.hpp;${CMAKE_CURRENT_SOURCE_DIR}/prompt.hpp;${CMAKE_CURRENT_SOURCE_DIR}/window.hpp;${CMAKE_CURRENT_SOURCE_DIR}/version.hpp"
                      PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/platforms/conversion.hpp;${CMAKE_CURRENT_SOURCE_DIR}/platforms/macros.hpp")

add_library(cpp-terminal::cpp-terminal ALIAS cpp-terminal)

# Install
if (CPPTERMINAL_ENABLE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/cpp-terminalConfig.cmake.in" "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" COMPATIBILITY AnyNewerVersion)
  install(TARGETS cpp-terminal cpp-terminal-platforms cpp-terminalWarnings EXPORT cpp-terminalTargets
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpp-terminal"
          PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpp-terminal/platforms")
  install(EXPORT cpp-terminalTargets FILE cpp-terminalTargets.cmake NAMESPACE cpp-terminal:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  install(FILES "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
endif()
