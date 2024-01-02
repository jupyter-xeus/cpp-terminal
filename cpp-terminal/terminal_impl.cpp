/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal_impl.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/private/exception.hpp"
#include "cpp-terminal/private/file.hpp"
#include "cpp-terminal/private/sigwinch.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"  //FIXME avoid recursion

Term::Options Term::Terminal::getOptions() const noexcept { return m_options; }

Term::Terminal::Terminal() noexcept
try
{
  Term::Private::Sigwinch::blockSigwinch();
  Term::Private::Sigwinch::registerSigwinch();
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
  if(getOptions().has(Option::ClearScreen)) { Term::Private::out.write(clear_buffer() + style(Style::Reset) + cursor_move(1, 1) + screen_load()); }
  if(getOptions().has(Option::NoCursor)) { Term::Private::out.write(cursor_on()); }
  set_unset_utf8();
  store_and_restore();
  unsetFocusEvents();
  unsetMouseEvents();
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

void Term::Terminal::applyOptions() const
{
  if(getOptions().has(Option::ClearScreen)) { Term::Private::out.write(screen_save() + clear_buffer() + style(Style::Reset) + cursor_move(1, 1)); }
  if(getOptions().has(Option::NoCursor)) { Term::Private::out.write(cursor_off()); }
  setMode();
}
