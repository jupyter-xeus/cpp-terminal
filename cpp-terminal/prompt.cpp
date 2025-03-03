/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/prompt.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/private/conversion.hpp"
#include "cpp-terminal/private/macros.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"
#include "position.hpp"

#include <iostream>

Term::Result Term::prompt(const std::string& message, const std::string& first_option, const std::string& second_option, const std::string& prompt_indicator, bool immediate)
{
  Term::terminal.setOptions(Option::NoClearScreen, Option::NoSignalKeys, Option::Cursor, Term::Option::Raw);
  std::cout << message << " [" << first_option << '/' << second_option << ']' << prompt_indicator << ' ' << std::flush;

  if(!Term::is_stdin_a_tty())
  {
    std::cout << '\n' << std::flush;
    return Result::Error;
  }

  Term::Key key;

  if(immediate)
  {
    while(true)
    {
      key = Term::read_event();
      if(key == Term::Key::NoKey) continue;
      if(key == Term::Key::y || key == Term::Key::Y)
      {
        std::cout << '\n' << std::flush;
        return Result::Yes;
      }
      else if(key == Term::Key::n || key == Term::Key::N)
      {
        std::cout << '\n' << std::flush;
        return Result::No;
      }
      else if(key == Term::Key::Ctrl_C || key == Term::Key::Ctrl_D)
      {
        std::cout << '\n' << std::flush;
        return Result::Abort;
      }
      else if(key == Term::Key::Enter)
      {
        std::cout << '\n' << std::flush;
        return Result::None;
      }
      else
      {
        std::cout << '\n' << std::flush;
        return Result::Invalid;
      }
    }
  }
  else
  {
    std::string input;
    while(true)
    {
      key = Term::read_event();
      if(key == Term::Key::NoKey) continue;
      if(key >= 'a' && key <= 'z')
      {
        std::cout << (char)key << std::flush;
        input.push_back(static_cast<char>(key));
      }
      else if(key >= 'A' && key <= 'Z')
      {
        std::cout << (char)key << std::flush;
        input.push_back(static_cast<char>(key.tolower()));  // convert upper case to lowercase
      }
      else if(key == Term::Key::Ctrl_C || key == Term::Key::Ctrl_D)
      {
        std::cout << '\n';
        return Result::Abort;
      }
      else if(key == Term::Key::Backspace)
      {
        if(input.empty() != 0)
        {
          std::cout << "\u001b[D \u001b[D" << std::flush;  // erase last line and move the cursor back
          input.pop_back();
        }
      }
      else if(key == Term::Key::Enter)
      {
        if(input == "y" || input == "yes")
        {
          std::cout << '\n' << std::flush;
          return Result::Yes;
        }
        else if(input == "n" || input == "no")
        {
          std::cout << '\n' << std::flush;
          return Result::No;
        }
        else if(input.empty())
        {
          std::cout << '\n' << std::flush;
          return Result::None;
        }
        else
        {
          std::cout << '\n' << std::flush;
          return Result::Invalid;
        }
      }
    }
  }
}

Term::Result_simple Term::prompt_simple(const std::string& message)
{
  switch(prompt(message, "y", "N", ":", false))
  {
    case Result::Yes: return Result_simple::Yes;
    case Result::Abort: return Result_simple::Abort;
    case Result::No:     // falls through
    case Result::Error:  // falls through
    case Result::None:   // falls through
    case Result::Invalid:
    default: return Result_simple::No;
  }
}

std::string Term::concat(const std::vector<std::string>& lines)
{
  std::string s;
  for(auto& line: lines) { s.append(line + "\n"); }
  return s;
}

std::vector<std::string> Term::split(const std::string& s)
{
  std::size_t              j = 0;
  std::vector<std::string> lines;
  lines.emplace_back("");
  if(s[s.size() - 1] != '\n') throw Term::Exception("\\n is required");
  for(std::size_t i = 0; i < s.size() - 1; i++)
  {
    if(s[i] == '\n')
    {
      j++;
      lines.emplace_back("");
    }
    else { lines[j].push_back(s[i]); }
  }
  return lines;
}

char32_t UU(const std::string& s)
{
  std::u32string s2 = Term::Private::utf8_to_utf32(s);
  if(s2.size() != 1) throw Term::Exception("U(s): s not a codepoint.");
  return s2[0];
}

void Term::print_left_curly_bracket(Term::Window& scr, const std::size_t& x, const std::size_t& y1, const std::size_t& y2)
{
  std::size_t h{y2 - y1 + 1};
  if(h == 1) { scr.set_char(x, y1, UU("]")); }
  else
  {
    scr.set_char(x, y1, UU("┐"));
    for(std::size_t j = y1 + 1; j <= y2 - 1; j++) { scr.set_char(x, j, UU("│")); }
    scr.set_char(x, y2, UU("┘"));
  }
}

void Term::render(Term::Window& scr, const Model& model, const std::size_t& cols)
{
  scr.clear();
  print_left_curly_bracket(scr, cols, 1, model.lines.size());
  scr.print_str(cols - 6, model.lines.size(), std::to_string(model.cursor_row) + "," + std::to_string(model.cursor_col));
  for(std::size_t j = 0; j < model.lines.size(); j++)
  {
    if(j == 0)
    {
      scr.fill_fg(1, j + 1, model.prompt_string.size(), model.lines.size(), Term::Color::Name::Green);
      scr.fill_style(1, j + 1, model.prompt_string.size(), model.lines.size(), Term::Style::Bold);
      scr.print_str(1, j + 1, model.prompt_string);
    }
    else
    {
      for(std::size_t i = 0; i < model.prompt_string.size() - 1; i++) { scr.set_char(i + 1, j + 1, '.'); }
    }
    scr.print_str(model.prompt_string.size() + 1, j + 1, model.lines[j]);
  }
  scr.set_cursor_pos(model.prompt_string.size() + model.cursor_col, model.cursor_row);
}

std::string Term::prompt_multiline(const std::string& prompt_string, std::vector<std::string>& m_history, std::function<bool(std::string)>& iscomplete)
{
  Term::Cursor cursor;
  Term::Screen screen({Term::Rows(25), Term::Columns(80)});
  bool         term_attached = Term::is_stdin_a_tty();
  if(is_stdin_a_tty())
  {
    cursor = cursor_position();
    screen = screen_size();
  }

  Model model;
  model.prompt_string = prompt_string;

  // Make a local copy of history that can be modified by the user. All
  // changes will be forgotten once a command is submitted.
  std::vector<std::string> history     = m_history;
  std::size_t              history_pos = history.size();
  history.push_back(concat(model.lines));  // Push back empty input

  Term::Window scr({Term::Columns(screen.columns()), Term::Rows(1)});
  Term::Key    key;
  render(scr, model, screen.columns());
  std::cout << scr.render(1, cursor.row(), term_attached) << std::flush;
  bool not_complete = true;
  while(not_complete)
  {
    key = Term::read_event();
    if(key == Term::Key::NoKey) continue;
    if(key.isprint())
    {
      std::string before = model.lines[model.cursor_row - 1].substr(0, model.cursor_col - 1);
      std::string newchar;
      newchar.push_back(static_cast<char>(key));
      std::string after                 = model.lines[model.cursor_row - 1].substr(model.cursor_col - 1);
      model.lines[model.cursor_row - 1] = before += newchar += after;
      model.cursor_col++;
    }
    else if(key == Key::Ctrl_D)
    {
      if(model.lines.size() == 1 && model.lines[model.cursor_row - 1].empty())
      {
        model.lines[model.cursor_row - 1].push_back(static_cast<char>(Key::Ctrl_D));
        std::cout << "\n" << std::flush;
        m_history.push_back(model.lines[0]);
        return model.lines[0];
      }
    }
    else
    {
      switch(key)
      {
        case Key::Enter:
          not_complete = !iscomplete(concat(model.lines));
          if(not_complete) key = Key(static_cast<Term::Key>(Term::MetaKey::Value::Alt + Term::Key::Enter));
          else
            break;
          CPP_TERMINAL_FALLTHROUGH;
        case Key::Backspace:
          if(model.cursor_col > 1)
          {
            std::string before                = model.lines[model.cursor_row - 1].substr(0, model.cursor_col - 2);
            std::string after                 = model.lines[model.cursor_row - 1].substr(model.cursor_col - 1);
            model.lines[model.cursor_row - 1] = before + after;
            model.cursor_col--;
          }
          else if(model.cursor_col == 1 && model.cursor_row > 1)
          {
            model.cursor_col = model.lines[model.cursor_row - 2].size() + 1;
            model.lines[model.cursor_row - 2] += model.lines[model.cursor_row - 1];
            model.lines.erase(model.lines.begin() + static_cast<long>(model.cursor_row) - 1);
            model.cursor_row--;
          }
          break;
        case Key::Del:
          if(model.cursor_col <= model.lines[model.cursor_row - 1].size())
          {
            std::string before                = model.lines[model.cursor_row - 1].substr(0, model.cursor_col - 1);
            std::string after                 = model.lines[model.cursor_row - 1].substr(model.cursor_col);
            model.lines[model.cursor_row - 1] = before + after;
          }
          break;
        case Key::ArrowLeft:
          if(model.cursor_col > 1) { model.cursor_col--; }
          break;
        case Key::ArrowRight:
          if(model.cursor_col <= model.lines[model.cursor_row - 1].size()) { model.cursor_col++; }
          break;
        case Key::Home: model.cursor_col = 1; break;
        case Key::End: model.cursor_col = model.lines[model.cursor_row - 1].size() + 1; break;
        case Key::ArrowUp:
          if(model.cursor_row == 1)
          {
            if(history_pos > 0)
            {
              history[history_pos] = concat(model.lines);
              history_pos--;
              model.lines      = split(history[history_pos]);
              model.cursor_row = model.lines.size();
              if(model.cursor_col > model.lines[model.cursor_row - 1].size() + 1) { model.cursor_col = model.lines[model.cursor_row - 1].size() + 1; }
              if(model.lines.size() > scr.columns()) { scr.set_h(model.lines.size()); }
            }
          }
          else
          {
            model.cursor_row--;
            if(model.cursor_col > model.lines[model.cursor_row - 1].size() + 1) { model.cursor_col = model.lines[model.cursor_row - 1].size() + 1; }
          }
          break;
        case Key::ArrowDown:
          if(model.cursor_row == model.lines.size())
          {
            if(history_pos < history.size() - 1)
            {
              history[history_pos] = concat(model.lines);
              history_pos++;
              model.lines      = split(history[history_pos]);
              model.cursor_row = 1;
              if(model.cursor_col > model.lines[model.cursor_row - 1].size() + 1) { model.cursor_col = model.lines[model.cursor_row - 1].size() + 1; }
              if(model.lines.size() > scr.columns()) { scr.set_h(model.lines.size()); }
            }
          }
          else
          {
            model.cursor_row++;
            if(model.cursor_col > model.lines[model.cursor_row - 1].size() + 1) { model.cursor_col = model.lines[model.cursor_row - 1].size() + 1; }
          }
          break;
        case Key::Ctrl_N:
        {
          std::string before                = model.lines[model.cursor_row - 1].substr(0, model.cursor_col - 1);
          std::string after                 = model.lines[model.cursor_row - 1].substr(model.cursor_col - 1);
          model.lines[model.cursor_row - 1] = before;
          if(model.cursor_row < model.lines.size())
          {
            // Not at the bottom row, can't push back
            model.lines.insert(model.lines.begin() + static_cast<long>(model.cursor_row), after);
          }
          else { model.lines.push_back(after); }
          model.cursor_col = 1;
          model.cursor_row++;
          if(model.lines.size() > scr.columns()) { scr.set_h(model.lines.size()); }
          break;
        }
        default: break;
      }
    }
    render(scr, model, screen.columns());
    std::cout << scr.render(1, cursor.row(), term_attached) << std::flush;
    if(cursor.row() + scr.columns() - 1 > screen.rows())
    {
      cursor = Cursor({Row(static_cast<std::uint16_t>(screen.rows() - (scr.columns() - 1))), Column(cursor.column())});
      std::cout << scr.render(1, cursor.row(), term_attached) << std::flush;
    }
  }
  std::string line_skips;
  for(std::size_t i = 0; i <= model.lines.size() - model.cursor_row; i++) { line_skips += "\n"; }
  std::cout << line_skips << std::flush;
  m_history.push_back(concat(model.lines));
  return concat(model.lines);
}
