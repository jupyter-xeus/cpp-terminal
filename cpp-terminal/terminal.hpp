/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <cstdint>

namespace Term
{

class Terminal
{
private:
  void           store_and_restore();
  void           setOptions();
  void           applyOptions();
  void           setMode();
  std::int16_t   setMouseEvents();
  std::int16_t   unsetMouseEvents();
  std::int16_t   setFocusEvents();
  std::int16_t   unsetFocusEvents();
  void           set_unset_utf8();
  Term::Terminfo m_terminfo;
  Term::Options  m_options;

public:
  Terminal();
  ~Terminal();
  template<typename... Args> void setOptions(const Args&&... args)
  {
    m_options = {args...};
    applyOptions();
  }

  Term::Options getOptions();

  Terminal(const Terminal&)            = delete;
  Terminal& operator=(const Terminal&) = delete;
  Terminal(Terminal&&)                 = delete;
  Terminal& operator=(Terminal&&)      = delete;
  bool      supportUTF8();
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
