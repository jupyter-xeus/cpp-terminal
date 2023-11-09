/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

#include <array>

namespace
{
std::array<char, sizeof(Term::TOstream)> cout_buffer;
std::array<char, sizeof(Term::TOstream)> clog_buffer;
std::array<char, sizeof(Term::TOstream)> cerr_buffer;
std::array<char, sizeof(Term::TOstream)> cin_buffer;
}  // namespace

Term::TOstream& Term::cout = reinterpret_cast<Term::TOstream&>(cout_buffer);
Term::TOstream& Term::clog = reinterpret_cast<Term::TOstream&>(clog_buffer);
Term::TOstream& Term::cerr = reinterpret_cast<Term::TOstream&>(cerr_buffer);
Term::TIstream& Term::cin  = reinterpret_cast<Term::TIstream&>(cin_buffer);
