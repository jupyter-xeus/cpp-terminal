#include "cpp-terminal/input.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

int main()
{
  Term::Terminal term({Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor});
  std::cout << "Input from stdin: " << Term::read_stdin() << std::endl;
}
