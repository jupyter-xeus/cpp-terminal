/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

#ifdef _WIN32
  #include <windows.h>
#endif

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main()
#endif
{
  std::cout << Term::color_fg(Term::Color::Name::Red) << "Hello world !" << color_fg(Term::Color::Name::Default) << std::endl;
#ifdef _WIN32
  MessageBox(NULL, "Hello, world", "caption", 0);
#endif
  return 0;
}
