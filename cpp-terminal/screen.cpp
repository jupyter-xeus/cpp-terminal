/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/screen.hpp"

Term::Screen::Screen(const Term::Size& size) : m_size(size) {}

const Term::Rows& Term::Screen::rows() const noexcept { return m_size.rows(); }

const Term::Columns& Term::Screen::columns() const noexcept { return m_size.columns(); }

bool Term::Screen::empty() const { return (0 == m_size.rows()) && (0 == m_size.columns()); }

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
