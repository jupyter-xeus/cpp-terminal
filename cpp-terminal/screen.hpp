/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/size.hpp"

#include <string>

namespace Term
{

class Screen
{
public:
  Screen() = default;
  explicit Screen(const Size& size);
  const Rows&    rows() const noexcept;
  const Columns& columns() const noexcept;
  bool           empty() const;
  bool           operator==(const Term::Screen& screen) const;
  bool           operator!=(const Term::Screen& screen) const;

private:
  Size m_size;
};

// clear the screen
std::string clear_screen();
// save the current terminal state
std::string screen_save();
// load a previously saved terminal state
std::string screen_load();
// get the terminal size
Screen      screen_size();

}  // namespace Term
