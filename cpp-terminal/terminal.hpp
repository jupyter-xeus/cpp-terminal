/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminfo.hpp"

namespace Term
{

class Terminal
{
public:
  Terminal();
  ~Terminal();
  template<typename Arg1 = Term::Option, typename... Args> void setOptions(Arg1 arg1, Args... args)
  {
    m_options.set(arg1);
    setOptions(args...);
  }

  void setOptions(const Term::Options& options);

  Term::Options getOptions();

  Terminal(const Terminal&)            = delete;
  Terminal& operator=(const Terminal&) = delete;
  Terminal(Terminal&&)                 = delete;
  Terminal& operator=(Terminal&&)      = delete;
  bool      supportUTF8();

private:
  void           store_and_restore();
  void           setBadStateReturnCode();
  void           setOptions();
  void           applyOptions();
  void           setRawMode();
  int            activateMouseEvents();
  int            desactivateMouseEvents();
  int            activateFocusEvents();
  int            desactivateFocusEvents();
  void           set_unset_utf8();
  Term::Terminfo m_terminfo;
  Term::Options  m_options;
  std::uint8_t   m_badReturnCode{EXIT_FAILURE};
};

class TerminalInitializer
{
public:
  TerminalInitializer();
  static void init();
  ~TerminalInitializer();
  TerminalInitializer(const TerminalInitializer&)            = delete;
  TerminalInitializer& operator=(const TerminalInitializer&) = delete;
  TerminalInitializer(TerminalInitializer&&)                 = delete;
  TerminalInitializer& operator=(TerminalInitializer&&)      = delete;

private:
  static int m_counter;
};

static TerminalInitializer m_terminalInitializer;
extern Term::Terminal&     terminal;

// change the title of the terminal, only supported by a few terminals
std::string terminal_title(const std::string& title);
// clear the screen and the scroll-back buffer
std::string clear_buffer();

}  // namespace Term
