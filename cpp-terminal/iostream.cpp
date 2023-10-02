/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

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

int Term::StreamInitializer::m_counter{0};

void Term::StreamInitializer::init()
{
  if(m_counter++ == 0)
  {
    std::ios_base::Init();
    new(&Term::cout) TOstream(Term::Buffer::Type::FullBuffered, BUFSIZ);
    new(&Term::clog) TOstream(Term::Buffer::Type::LineBuffered, BUFSIZ);
    new(&Term::cerr) TOstream(Term::Buffer::Type::Unbuffered, 0);
    new(&Term::cin) TIstream(Term::Buffer::Type::FullBuffered, BUFSIZ);
    std::cin.rdbuf(Term::cin.rdbuf());
  }
}

Term::StreamInitializer::StreamInitializer() { init(); }

Term::StreamInitializer::~StreamInitializer()
{
  if(--m_counter == 0)
  {
    (&Term::cout)->~TOstream();
    (&Term::cerr)->~TOstream();
    (&Term::clog)->~TOstream();
    (&Term::cin)->~TIstream();
  }
}
