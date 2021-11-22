# configure version information
configure_file(version.h.in version.h)

# create and confirgure library target
add_library(cpp-terminal terminal_base.cpp terminal.cpp base.cpp prompt.cpp window.cpp)

target_include_directories(cpp-terminal BEFORE PUBLIC
    ${cpp-terminal_SOURCE_DIR}
)

target_include_directories(cpp-terminal INTERFACE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

set_target_properties(cpp-terminal PROPERTIES
    PUBLIC_HEADER "cpp-terminal/terminal.h;cpp-terminal/terminal_base.h;cpp-terminal/version.h;cpp-terminal/base.hpp;cpp-terminal/prompt.hpp;cpp-terminal/window.hpp"
)
