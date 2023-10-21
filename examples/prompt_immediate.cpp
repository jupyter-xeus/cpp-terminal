/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/version.hpp"

int main()
{
  Term::cout << "Running cpp-terminal version: " << Term::Version::string << std::endl;
  try
  {
    Term::cout << "CPP-Terminal basic prompt example: \n\n";
    switch(Term::prompt("Proceed?", "Y", "n", ":", true))
    {
      case Term::Result::NONE:  // no input was given
        Term::cout << "No input given, proceeding anyway...\n";
        break;
      case Term::Result::INVALID: Term::cout << "Invalid input given, proceeding anyway\n"; break;
      case Term::Result::YES: Term::cout << "Proceeding...\n"; break;
      case Term::Result::NO: Term::cout << "Stopping...\n"; break;
      case Term::Result::ABORT: Term::cout << "Exit signal received, exiting now...\n"; break;
      case Term::Result::ERROR:
        Term::cout << "Error while capturing input, is your terminal attached to a TTY?\n";
        Term::cout << "Aborting...\n";
        break;
    }
  }
  catch(const Term::Exception& re)
  {
    Term::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    Term::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
