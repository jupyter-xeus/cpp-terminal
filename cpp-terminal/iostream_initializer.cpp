/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream_initializer.hpp"

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/private/exception.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/terminal_initializer.hpp"
#include "cpp-terminal/tty.hpp"

#include <cstddef>
#include <iostream>

std::size_t Term::IOStreamInitializer::m_counter{0};

Term::IOStreamInitializer::IOStreamInitializer() noexcept
try
{
  if(0 == m_counter)
  {
    static const std::ios_base::Init       iostreams_init;  // Init std::cout etc...
    static const Term::TerminalInitializer terminal_init;   // Make sure terminal is set up.
    new(&Term::cout) TOstream(Term::Buffer::Type::FullBuffered, BUFSIZ);
    new(&Term::clog) TOstream(Term::Buffer::Type::LineBuffered, BUFSIZ);
    new(&Term::cerr) TOstream(Term::Buffer::Type::Unbuffered, 0);
    new(&Term::cin) TIstream(Term::Buffer::Type::FullBuffered, BUFSIZ);
    if(is_stdin_a_tty()) { std::cin.rdbuf(Term::cin.rdbuf()); }
  }
  ++m_counter;
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

Term::IOStreamInitializer::~IOStreamInitializer() noexcept
try
{
  --m_counter;
  if(0 == m_counter)
  {
    (&Term::cout)->~TOstream();
    (&Term::cerr)->~TOstream();
    (&Term::clog)->~TOstream();
    (&Term::cin)->~TIstream();
  }
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}
