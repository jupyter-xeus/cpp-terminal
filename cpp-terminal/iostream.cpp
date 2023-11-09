/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

#include "cpp-terminal/buffer.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <new>

namespace Term
{
// Output
static char coutBuf[sizeof(Term::TOstream)];
TOstream&   cout = reinterpret_cast<Term::TOstream&>(coutBuf);
static char cerrBuf[sizeof(Term::TOstream)];
TOstream&   cerr = reinterpret_cast<Term::TOstream&>(cerrBuf);
static char clogBuf[sizeof(Term::TOstream)];
TOstream&   clog = reinterpret_cast<Term::TOstream&>(clogBuf);
// Input
static char cinBuf[sizeof(Term::TIstream)];
TIstream&   cin = reinterpret_cast<Term::TIstream&>(cinBuf);
}  // namespace Term */
