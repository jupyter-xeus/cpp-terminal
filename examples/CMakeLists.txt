# Suppress useless warnings in examples but useful in cpp-terminal
add_library(ExamplesWarnings INTERFACE)
target_compile_options(
        ExamplesWarnings
        INTERFACE
        $<$<C_COMPILER_ID:MSVC>:/wd4061 /utf-8 /wd4668>
        $<$<CXX_COMPILER_ID:MSVC>:/wd4061 /utf-8 /wd4668>
)
add_library(Warnings::Examples ALIAS ExamplesWarnings)

# Create an example
function(cppterminal_example)
  cmake_parse_arguments(ARG "WIN32" "SOURCE" "" ${ARGN})
  if(ARG_WIN32)
    add_executable("${ARG_SOURCE}" WIN32 "${ARG_SOURCE}.cpp")
  else()
    add_executable("${ARG_SOURCE}" "${ARG_SOURCE}.cpp")
  endif()
  target_link_libraries("${ARG_SOURCE}" PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings Warnings::Examples)
  if(CPPTERMINAL_ENABLE_INSTALL)
    install(TARGETS "${ARG_SOURCE}" RUNTIME DESTINATION bin/examples)
  endif()
endfunction()

cppterminal_example(SOURCE args)
cppterminal_example(SOURCE cin_cooked)
cppterminal_example(SOURCE cin_raw)
cppterminal_example(SOURCE colors)
cppterminal_example(SOURCE cout)
cppterminal_example(SOURCE events)
cppterminal_example(SOURCE keys)
cppterminal_example(SOURCE kilo)
cppterminal_example(SOURCE menu)
cppterminal_example(SOURCE menu_window)
cppterminal_example(SOURCE minimal)
cppterminal_example(SOURCE prompt_immediate)
cppterminal_example(SOURCE prompt_multiline)
cppterminal_example(SOURCE prompt_not_immediate)
cppterminal_example(SOURCE prompt_simple)
cppterminal_example(SOURCE styles)
cppterminal_example(SOURCE slow_events)
cppterminal_example(SOURCE utf8)
cppterminal_example(SOURCE attach_console WIN32)
cppterminal_example(SOURCE attach_console_minimal WIN32)
