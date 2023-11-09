/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal_impl.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/private/file.hpp"
#include "cpp-terminal/private/return_code.hpp"
#include "cpp-terminal/private/sigwinch.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"

Term::Options Term::Terminal::getOptions() const noexcept { return m_options; }

Term::Terminal::Terminal()
{
  Term::Private::Sigwinch::blockSigwinch();
  store_and_restore();
  setMode();  //Save the default cpp-terminal mode done in store_and_restore();
  set_unset_utf8();
  m_terminfo.checkUTF8();
}

bool Term::Terminal::supportUTF8() { return m_terminfo.hasUTF8(); }

Term::Terminal::~Terminal()
{
  try
  {
    if(m_options.has(Option::ClearScreen)) { Term::Private::out.write(clear_buffer() + style(Style::Reset) + cursor_move(1, 1) + screen_load()); }
    if(m_options.has(Option::NoCursor)) { Term::Private::out.write(cursor_on()); }
    set_unset_utf8();
    store_and_restore();
    unsetFocusEvents();
    unsetMouseEvents();
  }
  catch(const Term::Exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\nreason : " + std::string(e.what()) + "\r\n");
    std::exit(Term::returnCode());
  }
  catch(const std::exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\nreason : " + std::string(e.what()) + "\r\n");
    std::exit(Term::returnCode());
  }
  catch(...)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\n");
    std::exit(Term::returnCode());
  }
}

void Term::Terminal::applyOptions()
{
  if(m_options.has(Option::ClearScreen)) { Term::Private::out.write(screen_save() + clear_buffer() + style(Style::Reset) + cursor_move(1, 1)); }
  if(m_options.has(Option::NoCursor)) { Term::Private::out.write(cursor_off()); }
  setMode();
}
