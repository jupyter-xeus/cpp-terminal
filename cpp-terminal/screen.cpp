/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/screen.hpp"

Term::Screen::Screen(const std::size_t& rows, const std::size_t& columns) : m_size({rows, columns}) {}

std::size_t Term::Screen::rows() const { return m_size.first; }

std::size_t Term::Screen::columns() const { return m_size.second; }

bool Term::Screen::empty() const { return (0 == m_size.second) && (0 == m_size.first); }

std::string Term::clear_screen() { return "\u001b[2J"; }

std::string Term::screen_save()
{
  return "\u001b7\u001b[?1049h";  // save current cursor position, save screen FIXME
}

std::string Term::screen_load()
{
  return "\u001b[?1049l\u001b8";  // restores screen, restore current cursor position FIXME
}

bool Term::Screen::operator==(const Term::Screen& screen) const { return (this->rows() == screen.rows()) && (this->columns() == screen.columns()); }

bool Term::Screen::operator!=(const Term::Screen& screen) const { return !(*this == screen); }
