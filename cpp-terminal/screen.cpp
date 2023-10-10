/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/screen.hpp"

std::size_t Term::Screen::rows() const { return m_size.first; }

std::size_t Term::Screen::columns() const { return m_size.second; }

bool Term::Screen::empty() const
{
  if(m_size.second == 0 && m_size.first == 0) return true;
  else
    return false;
}

std::string Term::clear_screen() { return "\x1b[2J"; }

std::string Term::screen_save()
{
  return static_cast<std::string>("\x1b") + static_cast<std::string>("7\x1b[?1049h");  // save current cursor position, save screen FIXME
}

std::string Term::screen_load()
{
  return static_cast<std::string>("\x1b[?1049l\x1b") + static_cast<std::string>("8");  // restores screen, restore current cursor position FIXME
}

bool Term::Screen::operator==(const Term::Screen& screen) const
{
  if(this->rows() == screen.rows() && this->columns() == screen.columns()) return true;
  else
    return false;
}

bool Term::Screen::operator!=(const Term::Screen& screen) const { return !(*this == screen); }
