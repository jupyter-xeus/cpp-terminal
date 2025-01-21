
/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminal.hpp"

#include <csignal>
#include <fstream>
#include <iostream>

int main()
{
  Term::terminal.setOptions(Term::Option::Raw, Term::Option::SignalKeys, Term::Option::ClearScreen);
  std::signal(SIGINT,
              [](int)
              {
                std::ofstream outfile("cpp_terminal_signal.txt");
                outfile << "my text here!" << std::endl;
                outfile.close();
                std::exit(1);
              });
  std::cout << "This is printed with std::cout with std::end (1)" << std::endl;
  std::clog << "This is printed with std::clog with std::end (2)" << std::endl;
  std::cerr << "This is printed with std::cerr with std::end (3)" << std::endl;

  Term::cout << "This is printed with Term::cout with std::end (4)" << std::endl;
  Term::clog << "This is printed with Term::clog with std::end (5)" << std::endl;
  Term::cerr << "This is printed with Term::cerr with std::end (6)" << std::endl;

  std::cout << "This is printed with std::cout with flush() (7)\n" << std::flush;
  std::clog << "This is printed with std::clog with flush() (8)\n" << std::flush;
  std::cerr << "This is printed with std::cerr with flush() (9)\n" << std::flush;

  Term::cout << "This is printed with Term::cout with flush() (10)\n" << std::flush;
  Term::clog << "This is printed with Term::clog with flush() (11)\n" << std::flush;
  Term::cerr << "This is printed with Term::cerr with flush() (12)\n" << std::flush;

  std::cout << "This is printed (13) ";
  std::cout << "with std::cout (13)";
  std::clog << "This is printed (14) ";
  std::clog << "with std::clog (14)";
  std::cerr << "This is printed (15) ";
  std::cerr << "with std::cerr (15)";
  while(true) {}
  return 0;
}
