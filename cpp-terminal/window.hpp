/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/platforms/macros.hpp"
#include "cpp-terminal/style.hpp"

#include <vector>

namespace Term
{

/* Represents a rectangular window, as a 2D array of characters and their
 * attributes. The render method can convert this internal representation to a
 * string that when printed will show the Window on the screen.
 *
 * Note: the characters are represented by char32_t, representing their UTF-32
 * code point. The natural way to represent a character in a terminal would be
 * a "unicode grapheme cluster", but due to a lack of a good library for C++
 * that could handle those, we simply use a Unicode code point as a character.
 */
class Window
{
private:
  std::size_t              w{0};
  std::size_t              h{0};  // width and height of the window
  std::size_t              cursor_x{1};
  std::size_t              cursor_y{1};  // current cursor position
  std::vector<char32_t>    chars;        // the characters in row first order
  std::vector<Term::Color> m_fg;
  std::vector<Term::Color> m_bg;
  std::vector<bool>        m_fg_reset;
  std::vector<bool>        m_bg_reset;
  std::vector<Style>       m_style;

  char32_t get_char(const std::size_t&, const std::size_t&);

  bool        get_fg_reset(const std::size_t&, const std::size_t&);
  bool        get_bg_reset(const std::size_t&, const std::size_t&);
  Term::Color get_fg(const std::size_t&, const std::size_t&);
  Term::Color get_bg(const std::size_t&, const std::size_t&);

  Term::Style get_style(const std::size_t&, const std::size_t&);

public:
  Window(const std::size_t& w, const std::size_t& h) : w{w}, h{h}, chars(w * h, ' '), m_fg(w * h, {0, 0, 0}), m_bg(w * h, {0, 0, 0}), m_fg_reset(w * h, true), m_bg_reset(w * h, true), m_style(w * h, Style::RESET){};

  CPP_TERMINAL_NODISCARD std::size_t get_w() const;

  CPP_TERMINAL_NODISCARD std::size_t get_h() const;

  void set_char(const std::size_t&, const std::size_t&, const char32_t&);

  void set_fg_reset(const std::size_t&, const std::size_t&);

  void set_bg_reset(const std::size_t&, const std::size_t&);

  void set_fg(const std::size_t&, const std::size_t&, const Color&);

  void set_bg(const std::size_t&, const std::size_t&, const Color&);

  void set_style(const std::size_t&, const std::size_t&, const Style&);

  void set_cursor_pos(const std::size_t&, const std::size_t&);

  void set_h(const std::size_t&);

  void print_str(const std::size_t&, const std::size_t&, const std::string&, const std::size_t& = 0, bool = false);

  void fill_fg(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&, const Color&);

  void fill_bg(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&, const Color&);

  void fill_style(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&, const Style&);

  void print_border();

  void print_rect(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&);

  void clear();

  // TODO: add Window/Screen parameter here, to be used like this:
  // old_scr = scr;
  // scr.print_str(...)
  // scr.render(1, 1, old_scr)
  std::string render(const std::size_t&, const std::size_t&, bool);
};

}  // namespace Term
