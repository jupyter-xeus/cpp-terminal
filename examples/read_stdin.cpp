#include "cpp-terminal/input.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

int main()
{
  Term::Terminal term(false, true, false);
  std::cout << "Input from stdin: " << Term::read_stdin() << std::endl;
}
