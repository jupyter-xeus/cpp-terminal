enable_testing()

add_executable(test_terminal test_terminal.cpp)
target_link_libraries(test_terminal cpp-terminal)
add_test(test_terminal ${PROJECT_BINARY_DIR}/test_terminal)
