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
#include "cpp-terminal/terminal_initializer.hpp"
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

}  // namespace Term
