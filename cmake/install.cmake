# Install
function(install_library)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/cpp-terminalConfig.cmake.in" "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" COMPATIBILITY AnyNewerVersion)
  install(TARGETS cpp-terminal cpp-terminalWarnings EXPORT cpp-terminalTargets
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpp-terminal"
          PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpp-terminal/private")
  install(EXPORT cpp-terminalTargets FILE cpp-terminalTargets.cmake NAMESPACE cpp-terminal:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
  install(FILES "${PROJECT_BINARY_DIR}/cmake/cpp-terminalConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cpp-terminalConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-terminal")
endfunction()
