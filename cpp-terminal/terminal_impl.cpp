/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal_impl.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/private/exception.hpp"
#include "cpp-terminal/private/file.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"

std::string Term::Terminal::clear() const noexcept { return "\u001b[3J"; }

Term::Options Term::Terminal::getOptions() const noexcept { return m_options; }

#ifdef _WIN32
  #pragma warning(push)
  #pragma warning(disable : 4297)
#endif

Term::Terminal::Terminal() noexcept
try
{
  store_and_restore();
  setMode();  //Save the default cpp-terminal mode done in store_and_restore();
  set_unset_utf8();
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

Term::Terminal::~Terminal() noexcept
try
{
  clean();
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

#ifdef _WIN32
  #pragma warning(pop)
#endif

void Term::Terminal::clean()
{
  unsetFocusEvents();
  unsetMouseEvents();
  if(getOptions().has(Option::NoCursor)) { Term::Private::out.write(cursor_on()); }
  if(getOptions().has(Option::ClearScreen)) { Term::Private::out.write(clear() + style(Style::Reset) + cursor_move(1, 1) + screen_load()); }
  set_unset_utf8();
  store_and_restore();
}

void Term::Terminal::applyOptions() const
{
  if(getOptions().has(Option::ClearScreen)) { Term::Private::out.write(screen_save() + clear() + style(Style::Reset) + cursor_move(1, 1)); }
  if(getOptions().has(Option::NoCursor)) { Term::Private::out.write(cursor_off()); }
  setMode();
}
