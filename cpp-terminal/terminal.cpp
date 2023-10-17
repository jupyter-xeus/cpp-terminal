/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/platforms/sigwinch.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"

#include <new>

namespace Term
{
static char termbuf[sizeof(Term::Terminal)];
Terminal&   terminal = reinterpret_cast<Term::Terminal&>(termbuf);
}  // namespace Term */

int Term::TerminalInitializer::m_counter{0};

void Term::TerminalInitializer::init()
{
  if(m_counter++ == 0) new(&Term::terminal) Terminal();
}

Term::TerminalInitializer::TerminalInitializer() { init(); }

Term::TerminalInitializer::~TerminalInitializer()
{
  if(--m_counter == 0) { (&Term::terminal)->~Terminal(); }
}

void Term::Terminal::setOptions(const Term::Options& options)
{
  m_options = options;
  applyOptions();
}

Term::Options Term::Terminal::getOptions() { return m_options; }

Term::Terminal::Terminal()
{
  Term::Private::Sigwinch::blockSigwinch();
  setBadStateReturnCode();
  Term::Private::m_fileInitializer.init();
  store_and_restore();
  set_unset_utf8();
  m_terminfo.checkUTF8();
}

bool Term::Terminal::supportUTF8() { return m_terminfo.hasUTF8(); }

Term::Terminal::~Terminal()
{
  try
  {
    if(m_options.has(Option::ClearScreen)) Term::Private::out.write(clear_buffer() + style(Style::RESET) + cursor_move(1, 1) + screen_load());
    if(m_options.has(Option::NoCursor)) Term::Private::out.write(cursor_on());
    set_unset_utf8();
    store_and_restore();
    desactivateFocusEvents();
    desactivateMouseEvents();
  }
  catch(const Term::Exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\nreason : " + std::string(e.what()) + "\r\n");
    std::exit(m_badReturnCode);
  }
  catch(const std::exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\nreason : " + std::string(e.what()) + "\r\n");
    std::exit(m_badReturnCode);
  }
  catch(...)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\r\n");
    std::exit(m_badReturnCode);
  }
}

void Term::Terminal::setOptions() { applyOptions(); }

void Term::Terminal::applyOptions()
{
  if(m_options.has(Option::ClearScreen)) Term::Private::out.write(screen_save() + clear_buffer() + style(Style::RESET) + cursor_move(1, 1));
  if(m_options.has(Option::NoCursor)) Term::Private::out.write(cursor_off());
  if(m_options.has(Option::Raw)) setRawMode();
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
