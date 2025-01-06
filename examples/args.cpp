/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/args.hpp"

#include <iostream>

int main()
{
  std::cout << "argc : " << Term::argc << std::endl;
  for(std::size_t i = 0; i != Term::argc; ++i) { std::cout << "argv[" << i << "] : *" << Term::argv[i] << "*" << std::endl; }
  return 0;
}
