add_executable(prompt_multiline prompt_multiline.cpp)
target_link_libraries(prompt_multiline PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(prompt_immediate prompt_immediate.cpp)
target_link_libraries(prompt_immediate PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(prompt_not_immediate prompt_not_immediate.cpp)
target_link_libraries(prompt_not_immediate PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(prompt_simple prompt_simple.cpp)
target_link_libraries(prompt_simple PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(kilo kilo.cpp)
target_link_libraries(kilo PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(menu menu.cpp)
target_link_libraries(menu PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(menu_window menu_window.cpp)
target_link_libraries(menu_window PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(keys keys.cpp)
target_link_libraries(keys PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(colors colors.cpp)
target_link_libraries(colors PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

add_executable(read_stdin read_stdin.cpp)
target_link_libraries(read_stdin PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings)

## Check if it's necessary
##target_compile_options(${target} PUBLIC "/utf-8")

if(CPPTERMINAL_ENABLE_INSTALL)
  install(TARGETS prompt_multiline prompt_immediate prompt_not_immediate prompt_simple kilo menu menu_window keys colors read_stdin RUNTIME DESTINATION bin/examples)
endif()
