# configure version information
configure_file(version.h.in version.h)

# create and configure library target
add_library(cpp-terminal base.cpp prompt.cpp window.cpp input.cpp private/platform.cpp)

target_include_directories(cpp-terminal PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/private> $<INSTALL_INTERFACE:include/cpp-terminal/private>
        PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include/cpp-terminal>)

set_target_properties(cpp-terminal PROPERTIES
        PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.h;${CMAKE_CURRENT_SOURCE_DIR}/base.hpp;${CMAKE_CURRENT_SOURCE_DIR}/prompt.hpp;${CMAKE_CURRENT_SOURCE_DIR}/window.hpp"
        PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/private/conversion.hpp;${CMAKE_CURRENT_SOURCE_DIR}/private/platform.hpp")

add_library(cpp-terminal::cpp-terminal ALIAS cpp-terminal)
