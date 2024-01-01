/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/window.hpp"

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/private/conversion.hpp"
#include "cpp-terminal/private/unicode.hpp"
#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <cstddef>

namespace Term
{

Term::Window::Window(const std::size_t& columns, const std::size_t& rows) : m_window({rows, columns}) { clear(); }

char32_t Term::Window::get_char(const std::size_t& column, const std::size_t& row) { return m_chars[index(column, row)]; }

bool Term::Window::get_fg_reset(const std::size_t& column, const std::size_t& row) { return m_fg_reset[index(column, row)]; }

bool Term::Window::get_bg_reset(const std::size_t& column, const std::size_t& row) { return m_bg_reset[index(column, row)]; }

Term::Color Term::Window::get_fg(const std::size_t& column, const std::size_t& row) { return m_fg[index(column, row)]; }

Term::Color Term::Window::get_bg(const std::size_t& column, const std::size_t& row) { return m_bg[index(column, row)]; }

Term::Style Term::Window::get_style(const std::size_t& column, const std::size_t& row) { return m_style[index(column, row)]; }

std::size_t Term::Window::get_w() const { return m_window.columns(); }

std::size_t Term::Window::get_h() const { return m_window.rows(); }

void Term::Window::set_char(const std::size_t& column, const std::size_t& row, const char32_t& character)
{
  if(insideWindow(column, row)) { m_chars[index(column, row)] = character; }
  else { throw Term::Exception("set_char(): (x,y) out of bounds"); }
}

void Term::Window::set_fg_reset(const std::size_t& column, const std::size_t& row)
{
  m_fg_reset[index(column, row)] = true;
  m_fg[index(column, row)]       = Term::Color::Name::Default;
}

void Term::Window::set_bg_reset(const std::size_t& column, const std::size_t& row)
{
  m_bg_reset[index(column, row)] = true;
  m_bg[index(column, row)]       = Term::Color::Name::Default;
}

void Term::Window::set_fg(const std::size_t& column, const std::size_t& row, const Color& color)
{
  m_fg_reset[index(column, row)] = false;
  m_fg[index(column, row)]       = color;
}

void Term::Window::set_bg(const std::size_t& column, const std::size_t& row, const Color& color)
{
  m_bg_reset[index(column, row)] = false;
  m_bg[index(column, row)]       = color;
}

void Term::Window::set_style(const std::size_t& column, const std::size_t& row, const Style& style) { m_style[index(column, row)] = style; }

void Term::Window::set_cursor_pos(const std::size_t& column, const std::size_t& row) { m_cursor = {row, column}; }

void Term::Window::set_h(const std::size_t& new_h)
{
  if(new_h == m_window.rows()) { return; }
  if(new_h > m_window.rows())
  {
    const std::size_t dc = (new_h - m_window.rows()) * m_window.columns();
    m_chars.insert(m_chars.end(), dc, ' ');
    m_fg_reset.insert(m_fg_reset.end(), dc, true);
    m_bg_reset.insert(m_bg_reset.end(), dc, true);
    m_fg.insert(m_fg.end(), dc, {0, 0, 0});
    m_bg.insert(m_bg.end(), dc, {0, 0, 0});
    m_style.insert(m_style.end(), dc, Style::Reset);
    m_window = {m_window.columns(), new_h};
  }
  else { throw Term::Exception("Shrinking height not supported."); }
}

void Term::Window::print_str(const std::size_t& x, const std::size_t& y, const std::string& s, const std::size_t& indent, bool move_cursor)
{
  std::u32string s2   = Private::utf8_to_utf32(s);
  std::size_t    xpos = x;
  std::size_t    ypos = y;
  for(char32_t i: s2)
  {
    if(i == U'\n')
    {
      xpos = x + indent;
      ypos++;
      if(insideWindow(xpos, ypos))
      {
        for(std::size_t j = 0; j < indent; ++j) { set_char(x + j, ypos, '.'); }
      }
      else { return; }
    }
    else
    {
      if(insideWindow(xpos, ypos)) { set_char(xpos, y, i); }
      else { return; }
      ++xpos;
    }
  }
  if(move_cursor) { m_cursor = {ypos, xpos}; }
}

void Term::Window::fill_fg(const std::size_t& x1, const std::size_t& y1, const std::size_t& x2, const std::size_t& y2, const Color& rgb)
{
  for(std::size_t j = y1; j <= y2; ++j)
  {
    for(std::size_t i = x1; i <= x2; ++i) { set_fg(i, j, rgb); }
  }
}

void Term::Window::fill_bg(const std::size_t& x1, const std::size_t& y1, const std::size_t& x2, const std::size_t& y2, const Color& rgb)
{
  for(std::size_t j = y1; j <= y2; ++j)
  {
    for(std::size_t i = x1; i <= x2; ++i) { set_bg(i, j, rgb); }
  }
}

void Term::Window::fill_style(const std::size_t& x1, const std::size_t& y1, const std::size_t& x2, const std::size_t& y2, const Style& color)
{
  for(std::size_t j = y1; j <= y2; ++j)
  {
    for(std::size_t i = x1; i <= x2; ++i) { set_style(i, j, color); }
  }
}

void Term::Window::print_border() { print_rect(1, 1, m_window.columns(), m_window.rows()); }

void Term::Window::print_rect(const std::size_t& x1, const std::size_t& y1, const std::size_t& x2, const std::size_t& y2)
{
  std::u32string border = Private::utf8_to_utf32("│─┌┐└┘");
  if(Term::Terminfo::get(Term::Terminfo::Bool::UTF8))
  {
    for(std::size_t j = y1 + 1; j <= (y2 - 1); ++j)
    {
      set_char(x1, j, border[0]);
      set_char(x2, j, border[0]);
    }
    for(std::size_t i = x1 + 1; i <= (x2 - 1); ++i)
    {
      set_char(i, y1, border[1]);
      set_char(i, y2, border[1]);
    }
    set_char(x1, y1, border[2]);
    set_char(x2, y1, border[3]);
    set_char(x1, y2, border[4]);
    set_char(x2, y2, border[5]);
  }
  else
  {
    for(std::size_t j = y1 + 1; j <= (y2 - 1); ++j)
    {
      set_char(x1, j, '|');
      set_char(x2, j, '|');
    }
    for(std::size_t i = x1 + 1; i <= (x2 - 1); ++i)
    {
      set_char(i, y1, '-');
      set_char(i, y2, '-');
    }
    set_char(x1, y1, '+');
    set_char(x2, y1, '+');
    set_char(x1, y2, '+');
    set_char(x2, y2, '+');
  }
}

void Term::Window::clear()
{
  const std::size_t area{m_window.rows() * m_window.columns()};
  m_style.assign(area, Style::Reset);
  m_bg_reset.assign(area, true);
  m_bg.assign(area, Term::Color::Name::Default);
  m_fg_reset.assign(area, true);
  m_fg.assign(area, Term::Color::Name::Default);
  m_chars.assign(area, ' ');
}

std::string Term::Window::render(const std::size_t& x0, const std::size_t& y0, bool term)
{
  std::string out;
  if(term) { out.append(cursor_off()); }
  Color current_fg       = Term::Color::Name::Default;
  Color current_bg       = Term::Color::Name::Default;
  bool  current_fg_reset = true;
  bool  current_bg_reset = true;
  Style current_style    = Style::Reset;
  for(std::size_t j = 1; j <= m_window.rows(); ++j)
  {
    if(term) { out.append(cursor_move(y0 + j - 1, x0)); }
    for(std::size_t i = 1; i <= m_window.columns(); ++i)
    {
      bool update_fg       = false;
      bool update_bg       = false;
      bool update_fg_reset = false;
      bool update_bg_reset = false;
      bool update_style    = false;
      if(current_fg_reset != get_fg_reset(i, j))
      {
        current_fg_reset = get_fg_reset(i, j);
        if(current_fg_reset)
        {
          update_fg_reset = true;
          current_fg      = {255, 255, 255};
        }
      }

      if(current_bg_reset != get_bg_reset(i, j))
      {
        current_bg_reset = get_bg_reset(i, j);
        if(current_bg_reset)
        {
          update_bg_reset = true;
          current_bg      = {255, 255, 255};
        }
      }

      if(!current_fg_reset)
      {
        if(!(current_fg == get_fg(i, j)))
        {
          current_fg = get_fg(i, j);
          update_fg  = true;
        }
      }

      if(!current_fg_reset)
      {
        if(!(current_bg == get_bg(i, j)))
        {
          current_bg = get_bg(i, j);
          update_bg  = true;
        }
      }
      if(current_style != get_style(i, j))
      {
        current_style = get_style(i, j);
        update_style  = true;
        if(current_style == Style::Reset)
        {
          // style::reset: reset fg and bg colors too, we have to
          // set them again if they are non-default, but if fg or
          // bg colors are reset, we do not update them, as
          // style::reset already did that.
          update_fg = !current_fg_reset;
          update_bg = !current_bg_reset;
        }
      }
      // Set style first, as style::reset will reset colors too
      if(update_style) { out.append(style(get_style(i, j))); }
      if(update_fg_reset) { out.append(color_fg(Term::Color::Name::Default)); }
      else if(update_fg)
      {
        const Term::Color color_tmp = get_fg(i, j);
        out.append(color_fg(color_tmp));
      }

      if(update_bg_reset) { out.append(color_bg(Term::Color::Name::Default)); }
      else if(update_bg)
      {
        const Term::Color color_tmp = get_bg(i, j);
        out.append(color_bg(color_tmp));
      }
      out.append(Private::utf32_to_utf8(get_char(i, j)));
    }
    if(j < m_window.rows()) { out.append("\n"); }
  }
  if(!current_fg_reset) { out.append(color_fg(Term::Color::Name::Default)); }
  if(!current_bg_reset) { out.append(color_bg(Term::Color::Name::Default)); }
  if(current_style != Style::Reset) { out.append(style(Style::Reset)); }
  if(term)
  {
    out.append(cursor_move(y0 + (m_cursor.row() - 1), x0 + (m_cursor.column() - 1)));
    out.append(cursor_on());
  }
  return out;
}

std::size_t Term::Window::index(const std::size_t& column, const std::size_t& row) const
{
  if(!insideWindow(column, row)) { throw Term::Exception("Cursor out of range"); }
  return ((row - 1) * m_window.columns()) + (column - 1);
}

bool Term::Window::insideWindow(const std::size_t& column, const std::size_t& row) const { return (column >= 1) && (row >= 1) && (column <= m_window.columns()) && (row <= m_window.rows()); }
}  // namespace Term
