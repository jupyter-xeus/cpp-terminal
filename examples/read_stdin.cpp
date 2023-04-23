#include "cpp-terminal/input.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  Term::terminal.setOptions({Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw});
  std::cout << "Input from stdin: " << Term::read_stdin() << std::endl;
}
