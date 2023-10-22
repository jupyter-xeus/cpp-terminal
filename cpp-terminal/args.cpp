/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/args.hpp"

namespace Term
{

Term::Argc::Argc() = default;

Term::Argc::operator unsigned int() { return static_cast<unsigned int>(Term::Arguments::argc()); }

Term::Argc::operator unsigned int() const { return static_cast<unsigned int>(Term::Arguments::argc()); }

Term::Arguments::Arguments() = default;

std::string Term::Arguments::operator[](const std::size_t& i) const { return m_args[i]; }

}  // namespace Term
