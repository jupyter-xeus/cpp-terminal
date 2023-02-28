#include "cpp-terminal/input.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

int main()
{
  Term::Terminal term(false, true, false);
  if(!Term::is_stdin_a_tty()) std::cout << "Input from stdin: " << Term::read_stdin() << std::endl;
  else { std::cout << "stdin is a tty" << std::endl; }
}
