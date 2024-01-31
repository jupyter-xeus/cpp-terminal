/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

#include <array>

namespace
{
std::array<char, sizeof(Term::TOstream)> cout_buffer;  //NOLINT(fuchsia-statically-constructed-objects)
std::array<char, sizeof(Term::TOstream)> clog_buffer;  //NOLINT(fuchsia-statically-constructed-objects)
std::array<char, sizeof(Term::TOstream)> cerr_buffer;  //NOLINT(fuchsia-statically-constructed-objects)
std::array<char, sizeof(Term::TIstream)> cin_buffer;   //NOLINT(fuchsia-statically-constructed-objects)
}  // namespace

Term::TOstream& Term::cout = reinterpret_cast<Term::TOstream&>(cout_buffer);  //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
Term::TOstream& Term::clog = reinterpret_cast<Term::TOstream&>(clog_buffer);  //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
Term::TOstream& Term::cerr = reinterpret_cast<Term::TOstream&>(cerr_buffer);  //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
Term::TIstream& Term::cin  = reinterpret_cast<Term::TIstream&>(cin_buffer);   //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
