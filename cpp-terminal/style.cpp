/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/style.hpp"

std::string Term::style(const Term::Style& style)
{
  //https://unix.stackexchange.com/questions/212933/background-color-whitespace-when-end-of-the-terminal-reached
  if(style == Term::Style::DefaultBackgroundColor) return "\033[" + std::to_string((std::uint8_t)style) + "m\033[K";
  else
    return "\033[" + std::to_string((std::uint8_t)style) + 'm';
}
