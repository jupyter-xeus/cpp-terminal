/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/style.hpp"

std::string Term::style(const Term::Style& style)
{
  //https://unix.stackexchange.com/questions/212933/background-color-whitespace-when-end-of-the-terminal-reached
  std::string ret{"\u001b[" + std::to_string(static_cast<std::uint8_t>(style)) + 'm'};
  if(style == Term::Style::DefaultBackgroundColor) { ret += "\u001b[K"; }
  return ret;
}
