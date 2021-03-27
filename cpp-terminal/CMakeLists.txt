set(CPP_TERMINAL_SOURCES base.cpp prompt.cpp tools.cpp window.cpp)
add_library(cpp-terminal ${CPP_TERMINAL_SOURCES})
target_include_directories(cpp-terminal INTERFACE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
set_target_properties(cpp-terminal PROPERTIES
    PUBLIC_HEADER "cpp-terminal/base.hpp;cpp-terminal/input.hpp;cpp-terminal/prompt.hpp;cpp-terminal/tools.hpp;cpp-terminal/window.hpp;cpp-terminal/version.hpp"
)

configure_file(version.hpp.in version.hpp)