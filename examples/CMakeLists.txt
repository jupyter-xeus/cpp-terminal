# Suppress useless warnings in examples but useful in cpp-terminal
add_library(ExamplesWarnings INTERFACE)
target_compile_options(
        ExamplesWarnings
        INTERFACE
        $<$<C_COMPILER_ID:MSVC>:/wd4061>
        $<$<CXX_COMPILER_ID:MSVC>:/wd4061>
)
add_library(Warnings::Examples ALIAS ExamplesWarnings)

# Create an example
function(cppterminal_example SOURCE)
  add_executable(${SOURCE} ${SOURCE}.cpp)
  target_link_libraries(${SOURCE} PRIVATE cpp-terminal::cpp-terminal Warnings::Warnings Warnings::Examples)
  if(CPPTERMINAL_ENABLE_INSTALL)
    install(TARGETS ${SOURCE} RUNTIME DESTINATION bin/examples)
  endif()
endfunction()

cppterminal_example(args)
cppterminal_example(cin_cooked)
cppterminal_example(cin_raw)
cppterminal_example(colors)
cppterminal_example(cout)
cppterminal_example(events)
cppterminal_example(keys)
cppterminal_example(kilo)
cppterminal_example(menu)
cppterminal_example(menu_window)
cppterminal_example(minimal)
cppterminal_example(prompt_immediate)
cppterminal_example(prompt_multiline)
cppterminal_example(prompt_not_immediate)
cppterminal_example(prompt_simple)
cppterminal_example(styles)
cppterminal_example(slow_events)
cppterminal_example(utf8)
