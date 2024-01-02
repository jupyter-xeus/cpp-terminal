/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal.hpp"

#include <array>

namespace
{
std::array<char, sizeof(Term::Terminal)> terminal_buffer;  //NOLINT(fuchsia-statically-constructed-objects)
}  // namespace

Term::Terminal& Term::terminal = reinterpret_cast<Term::Terminal&>(::terminal_buffer);  //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

std::string Term::terminal_title(const std::string& title) { return "\u001b]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\u001b[3J"; }
