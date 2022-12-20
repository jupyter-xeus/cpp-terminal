#include <cpp-terminal/input.hpp>
#include <exception>
#include <iostream>
int main() { std::cout << "Input from stdin: " << Term::read_stdin_alone() << std::endl; }
