/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"

#include <cstddef>
#include <vector>

namespace Term
{

///
/// @brief Represents a rectangular window, as a 2D array of characters and their attributes.
///
/// Represents a rectangular window, as a 2D array of characters and their attributes.
/// The render method can convert this internal representation to a string that when printed will show the Window on the screen.
//
// @note the characters are represented by char32_t, representing their UTF-32 code point.
/// The natural way to represent a character in a terminal would be a "unicode grapheme cluster", but due to a lack of a good library for C++ that could handle those, we simply use a Unicode code point as a character.
///
class Window
{
public:
  Window(const std::size_t& columns, const std::size_t& rows);

  std::size_t get_w() const;

  std::size_t get_h() const;

  void set_char(const std::size_t& column, const std::size_t& row, const char32_t& character);

  void set_fg_reset(const std::size_t& column, const std::size_t& row);

  void set_bg_reset(const std::size_t& column, const std::size_t& row);

  void set_fg(const std::size_t& column, const std::size_t& row, const Color& color);

  void set_bg(const std::size_t& column, const std::size_t& row, const Color& color);

  void set_style(const std::size_t& column, const std::size_t& row, const Style& style);

  void set_cursor_pos(const std::size_t& column, const std::size_t& row);

  void set_h(const std::size_t&);

  void print_str(const std::size_t& column, const std::size_t&, const std::string&, const std::size_t& = 0, bool = false);

  void fill_fg(const std::size_t& column, const std::size_t&, const std::size_t&, const std::size_t&, const Color&);

  void fill_bg(const std::size_t& column, const std::size_t&, const std::size_t&, const std::size_t&, const Color&);

  void fill_style(const std::size_t& column, const std::size_t&, const std::size_t&, const std::size_t&, const Style&);

  void print_border();

  void print_rect(const std::size_t& column, const std::size_t&, const std::size_t&, const std::size_t&);

  void clear();

  bool insideWindow(const std::size_t& column, const std::size_t& row) const;

  // TODO: add Window/Screen parameter here, to be used like this:
  // old_scr = scr;
  // scr.print_str(...)
  // scr.render(1, 1, old_scr)
  std::string render(const std::size_t&, const std::size_t&, bool);

private:
  std::size_t              index(const std::size_t& column, const std::size_t& row) const;
  Term::Screen             m_window{0, 0};
  Term::Cursor             m_cursor{1, 1};
  std::vector<char32_t>    m_chars;  // the characters in row first order
  std::vector<Term::Color> m_fg;
  std::vector<Term::Color> m_bg;
  std::vector<bool>        m_fg_reset;
  std::vector<bool>        m_bg_reset;
  std::vector<Style>       m_style;

  char32_t get_char(const std::size_t& column, const std::size_t& row);

  bool        get_fg_reset(const std::size_t& column, const std::size_t& row);
  bool        get_bg_reset(const std::size_t& column, const std::size_t& row);
  Term::Color get_fg(const std::size_t& column, const std::size_t& row);
  Term::Color get_bg(const std::size_t& column, const std::size_t& row);
  Term::Style get_style(const std::size_t& column, const std::size_t& row);
};

}  // namespace Term
