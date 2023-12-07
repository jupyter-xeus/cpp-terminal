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

#include <cstddef>

namespace Term
{

class Terminal
{
public:
  ~Terminal() noexcept;
  Terminal() noexcept;
  Terminal(const Terminal&)                                         = delete;
  Terminal(Terminal&&)                                              = delete;
  Terminal&                              operator=(Terminal&&)      = delete;
  Terminal&                              operator=(const Terminal&) = delete;
  template<typename... Args> static void setOptions(const Args&&... args)
  {
    m_options = {args...};
    applyOptions();
  }
  static Term::Options getOptions() noexcept;

private:
  ///
  ///@brief Store and restore the default state of the terminal. Configure the default mode for cpp-terminal.
  ///
  static void store_and_restore() noexcept;

  ///
  ///@brief Set mode raw/cooked.
  ///First call is to save the good state set-up by cpp-terminal.
  ///
  static void setMode();

  static void setOptions();
  static void applyOptions();

  static std::size_t setMouseEvents();
  static std::size_t unsetMouseEvents();
  static std::size_t setFocusEvents();
  static std::size_t unsetFocusEvents();

  static void          set_unset_utf8();
  static Term::Options m_options;
};

}  // namespace Term
