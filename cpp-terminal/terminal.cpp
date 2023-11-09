/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal.hpp"

#include <array>

namespace
{
std::array<char, sizeof(Term::Terminal)> terminal_buffer;
}  // namespace

Term::Terminal& Term::terminal = reinterpret_cast<Term::Terminal&>(::terminal_buffer);

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
