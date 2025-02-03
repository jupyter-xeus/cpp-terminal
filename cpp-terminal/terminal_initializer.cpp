/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal_initializer.hpp"

#include "cpp-terminal/private/exception.hpp"
#include "cpp-terminal/private/file_initializer.hpp"
#include "cpp-terminal/private/signals.hpp"
#include "cpp-terminal/terminal.hpp"

#include <new>

std::size_t Term::TerminalInitializer::m_counter{0};

#ifdef _WIN32
  #pragma warning(push)
  #pragma warning(disable : 4297)
#endif

Term::TerminalInitializer::TerminalInitializer() noexcept
try
{
  if(0 == m_counter)
  {
    static const Private::FileInitializer files_init;
    new(&Term::terminal) Terminal();
    static Term::Private::Signals signals(Term::terminal);
    sighandler_t                  handler = [](int signum) { signals.reset_and_raise(signum); };
    signals.setHandler(handler);
  }
  ++m_counter;
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

Term::TerminalInitializer::~TerminalInitializer() noexcept
try
{
  --m_counter;
  if(0 == m_counter) { (&Term::terminal)->~Terminal(); }
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

#ifdef _WIN32
  #pragma warning(pop)
#endif
