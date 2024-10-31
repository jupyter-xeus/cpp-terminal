
/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  std::at_quick_exit(
    []()
    {
      std::cout << "Unhandled exception\n" << std::flush;
      //std::abort();
    });
  std::set_terminate(
    []()
    {
      std::cout << "Unhandled exception\n" << std::flush;
      //std::abort();
    });
  Term::terminal.setOptions(Term::Option::Raw, Term::Option::SignalKeys);
  while(true) {}
  return 0;
}
