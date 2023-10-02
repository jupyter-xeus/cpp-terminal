/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

int main()
{
  Term::cout << "The Answer to the question is : " << 42 << '\n' << "Or more precisely : \033[31m" << double(42.01) << "\033[0m" << std::endl;
  Term::cerr << "The Answer to the question is : " << 42 << '\n' << "Or more precisely : \033[31m" << double(42.01) << "\033[0m" << std::endl;
  Term::clog << "The Answer to the question is : " << 42 << '\n' << "Or more precisely : \033[31m" << double(42.01) << "\033[0m" << std::endl;
}
