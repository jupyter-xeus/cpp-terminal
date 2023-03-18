# configure version information
configure_file(version.hpp.in version.hpp)

add_subdirectory(platforms)

# create and configure library target
add_library(cpp-terminal base.cpp prompt.cpp window.cpp input.cpp terminal.cpp color.cpp)
target_link_libraries(cpp-terminal PRIVATE Warnings::Warnings cpp-terminal::cpp-terminal-platforms)
target_compile_options(cpp-terminal PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(cpp-terminal PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)

set_target_properties(cpp-terminal PROPERTIES
                      PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.hpp;${CMAKE_CURRENT_SOURCE_DIR}/tty.hpp;${CMAKE_CURRENT_SOURCE_DIR}/terminal.hpp;${CMAKE_CURRENT_SOURCE_DIR}/base.hpp;${CMAKE_CURRENT_SOURCE_DIR}/prompt.hpp;${CMAKE_CURRENT_SOURCE_DIR}/window.hpp"
                      PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/platforms/conversion.hpp;${CMAKE_CURRENT_SOURCE_DIR}/platforms/macros.hpp")

add_library(cpp-terminal::cpp-terminal ALIAS cpp-terminal)
