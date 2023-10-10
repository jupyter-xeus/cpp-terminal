/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Term
{

class Screen
{
public:
  Screen() = default;
  Screen(const std::size_t& rows, const std::size_t& columns) { m_size = {rows, columns}; }
  std::size_t rows() const;
  std::size_t columns() const;
  bool        empty() const;
  bool        operator==(const Term::Screen& screen) const;
  bool        operator!=(const Term::Screen& screen) const;

private:
  std::pair<std::size_t, std::size_t> m_size;
};

// clear the screen
std::string clear_screen();
// save the current terminal state
std::string screen_save();
// load a previously saved terminal state
std::string screen_load();
// get the terminal size (row, column) / (Y, X)
Screen      screen_size();

}  // namespace Term
