function(install_library TARGET_LIB_NAME)

install(TARGETS ${TARGET_LIB_NAME} EXPORT ${TARGET_LIB_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/${TARGET_LIB_NAME}
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}ConfigVersion.cmake"
    VERSION ${CPPTERMINAL_VERSION}
    COMPATIBILITY AnyNewerVersion
)
export(EXPORT ${TARGET_LIB_NAME}Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}Targets.cmake"
)
set(ConfigPackageLocation lib/cmake/${TARGET_LIB_NAME})
configure_file(${TARGET_LIB_NAME}Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}Config.cmake"
)
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}ConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LIB_NAME}/${TARGET_LIB_NAME}Targets.cmake"
    DESTINATION
    ${ConfigPackageLocation}
    COMPONENT
    Devel
)    
endfunction(install_library TARGET_LIB_NAME)
