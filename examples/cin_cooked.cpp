/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  Term::terminal.setOptions(Term::Option::Cooked);
  std::cout << "Now type a number and then a string !" << std::endl;
  int         number{0};
  std::string string;
  Term::cin >> number;
  Term::cin >> string;
  std::cout << "Your number is " << number << " and string is " << string << std::endl;
  std::cout << "Now type a number and then a string !" << std::endl;
  number = 0;
  string.clear();
  std::cin >> number;
  std::cin >> string;
  std::cout << "Your number is " << number << " and string is " << string << std::endl;
  return 0;
}
