#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/private/conversion.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

Term::Result Term::prompt(const std::string& message, const std::string& first_option, const std::string& second_option, const std::string& prompt_indicator, bool immediate)
{
  Terminal term(false, true, true, false);
  std::cout << message << " [" << first_option << '/' << second_option << ']' << prompt_indicator << ' ' << std::flush;

  if(!Term::stdin_connected())
  {
    std::cout << '\n' << std::flush;
    return Result::ERROR;
  }

  int key;

  if(immediate)
  {
    while(true)
    {
      key = Term::read_key();
      if(key == 'y' || key == 'Y')
      {
        std::cout << '\n' << std::flush;
        return Result::YES;
      }
      else if(key == 'n' || key == 'N')
      {
        std::cout << '\n' << std::flush;
        return Result::NO;
      }
      else if(key == Term::Key::CTRL_C || key == Term::Key::CTRL_D)
      {
        std::cout << '\n' << std::flush;
        return Result::ABORT;
      }
      else if(key == Term::Key::ENTER)
      {
        std::cout << '\n' << std::flush;
        return Result::NONE;
      }
      else
      {
        std::cout << '\n' << std::flush;
        return Result::INVALID;
      }
    }
  }
  else
  {
    std::vector<char>  input;
    unsigned short int length = 0;
    while(true)
    {
      key = Term::read_key();
      if(key >= 'a' && key <= 'z')
      {
        std::cout << (char)key << std::flush;
        length++;
        input.push_back(static_cast<char>(key));
      }
      else if(key >= 'A' && key <= 'Z')
      {
        std::cout << (char)key << std::flush;
        length++;
        input.push_back(static_cast<char>(key + 32));  // convert upper case to lowercase
      }
      else if(key == Term::Key::CTRL_C || key == Term::Key::CTRL_D)
      {
        std::cout << '\n';
        return Result::ABORT;
      }
      else if(key == Term::Key::BACKSPACE)
      {
        if(length != 0)
        {
          std::cout << "\033[D \033[D" << std::flush;  // erase last line and move the cursor back
          length--;
          input.pop_back();
        }
      }
      else if(key == Term::Key::ENTER)
      {
        if(Private::vector_to_string(input) == "y" || Private::vector_to_string(input) == "yes")
        {
          std::cout << '\n' << std::flush;
          return Result::YES;
        }
        else if(Private::vector_to_string(input) == "n" || Private::vector_to_string(input) == "no")
        {
          std::cout << '\n' << std::flush;
          return Result::NO;
        }
        else if(length == 0)
        {
          std::cout << '\n' << std::flush;
          return Result::NONE;
        }
        else
        {
          std::cout << '\n' << std::flush;
          return Result::INVALID;
        }
      }
    }
  }
}

Term::Result_simple Term::prompt_simple(const std::string& message)
{
  switch(prompt(message, "y", "N", ":", false))
  {
    case Result::YES: return Result_simple::YES;
    case Result::ABORT: return Result_simple::ABORT;
    case Result::NO:     // falls through
    case Result::ERROR:  // falls through
    case Result::NONE:   // falls through
    case Result::INVALID: return Result_simple::NO;
  }
  // shouldn't be reached
  return Result_simple::NO;
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
  if(s[s.size() - 1] != '\n') throw std::runtime_error("\\n is required");
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

char32_t Term::UU(const std::string& s)
{
  std::u32string s2 = Private::utf8_to_utf32(s);
  if(s2.size() != 1) throw std::runtime_error("U(s): s not a codepoint.");
  return s2[0];
}

void Term::print_left_curly_bracket(Term::Window& scr, int x, int y1, int y2)
{
  int h = y2 - y1 + 1;
  if(h == 1) { scr.set_char(x, y1, UU("]")); }
  else
  {
    scr.set_char(x, y1, UU("┐"));
    for(int j = y1 + 1; j <= y2 - 1; j++) { scr.set_char(x, j, UU("│")); }
    scr.set_char(x, y2, UU("┘"));
  }
}

void Term::render(Term::Window& scr, const Model& m, std::size_t cols)
{
  scr.clear();
  print_left_curly_bracket(scr, cols, 1, m.lines.size());
  scr.print_str(cols - 6, m.lines.size(), std::to_string(m.cursor_row) + "," + std::to_string(m.cursor_col));
  for(std::size_t j = 0; j < m.lines.size(); j++)
  {
    if(j == 0)
    {
      scr.fill_fg(1, j + 1, m.prompt_string.size(), m.lines.size(), Term::Bit4_reference::GREEN);
      scr.fill_style(1, j + 1, m.prompt_string.size(), m.lines.size(), Term::Style::BOLD);
      scr.print_str(1, j + 1, m.prompt_string);
    }
    else
    {
      for(std::size_t i = 0; i < m.prompt_string.size() - 1; i++) { scr.set_char(i + 1, j + 1, '.'); }
    }
    scr.print_str(m.prompt_string.size() + 1, j + 1, m.lines[j]);
  }
  scr.set_cursor_pos(m.prompt_string.size() + m.cursor_col, m.cursor_row);
}

std::string Term::prompt_multiline(const std::string& prompt_string, std::vector<std::string>& m_history, std::function<bool(std::string)>& iscomplete)
{
  std::size_t row{1}, col{1};
  std::size_t rows{25}, cols{80};
  bool        term_attached = Term::is_stdin_a_tty();
  if(stdin_connected())
  {
    std::tie(row, col)   = cursor_position();
    std::tie(rows, cols) = get_size();
  }

  Model m;
  m.prompt_string = prompt_string;

  // Make a local copy of history that can be modified by the user. All
  // changes will be forgotten once a command is submitted.
  std::vector<std::string> history     = m_history;
  std::size_t              history_pos = history.size();
  history.push_back(concat(m.lines));  // Push back empty input

  Term::Window scr(cols, 1);
  Key          key{NO_KEY};
  render(scr, m, cols);
  std::cout << scr.render(1, row, term_attached) << std::flush;
  bool not_complete = true;
  while(not_complete)
  {
    key = static_cast<Key>(Term::read_key());
    if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (Term::is_extended_ASCII(key) && !iscntrl(key)))
    {
      std::string before = m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
      std::string newchar;
      newchar.push_back(key);
      std::string after         = m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
      m.lines[m.cursor_row - 1] = before += newchar += after;
      m.cursor_col++;
    }
    else if(key == Key::CTRL_D)
    {
      if(m.lines.size() == 1 && m.lines[m.cursor_row - 1].empty())
      {
        m.lines[m.cursor_row - 1].push_back(Key::CTRL_D);
        std::cout << "\n" << std::flush;
        m_history.push_back(m.lines[0]);
        return m.lines[0];
      }
    }
    else
    {
      switch(key)
      {
        case Key::BACKSPACE:
          if(m.cursor_col > 1)
          {
            std::string before        = m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 2);
            std::string after         = m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
            m.lines[m.cursor_row - 1] = before + after;
            m.cursor_col--;
          }
          else if(m.cursor_col == 1 && m.cursor_row > 1)
          {
            m.cursor_col = m.lines[m.cursor_row - 2].size() + 1;
            m.lines[m.cursor_row - 2] += m.lines[m.cursor_row - 1];
            m.lines.erase(m.lines.begin() + m.cursor_row - 1);
            m.cursor_row--;
          }
          break;
        case Key::DEL:
          if(m.cursor_col <= m.lines[m.cursor_row - 1].size())
          {
            std::string before        = m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
            std::string after         = m.lines[m.cursor_row - 1].substr(m.cursor_col);
            m.lines[m.cursor_row - 1] = before + after;
          }
          break;
        case Key::ARROW_LEFT:
          if(m.cursor_col > 1) { m.cursor_col--; }
          break;
        case Key::ARROW_RIGHT:
          if(m.cursor_col <= m.lines[m.cursor_row - 1].size()) { m.cursor_col++; }
          break;
        case Key::HOME: m.cursor_col = 1; break;
        case Key::END: m.cursor_col = m.lines[m.cursor_row - 1].size() + 1; break;
        case Key::ARROW_UP:
          if(m.cursor_row == 1)
          {
            if(history_pos > 0)
            {
              history[history_pos] = concat(m.lines);
              history_pos--;
              m.lines      = split(history[history_pos]);
              m.cursor_row = m.lines.size();
              if(m.cursor_col > m.lines[m.cursor_row - 1].size() + 1) { m.cursor_col = m.lines[m.cursor_row - 1].size() + 1; }
              if(m.lines.size() > scr.get_h()) { scr.set_h(m.lines.size()); }
            }
          }
          else
          {
            m.cursor_row--;
            if(m.cursor_col > m.lines[m.cursor_row - 1].size() + 1) { m.cursor_col = m.lines[m.cursor_row - 1].size() + 1; }
          }
          break;
        case Key::ARROW_DOWN:
          if(m.cursor_row == m.lines.size())
          {
            if(history_pos < history.size() - 1)
            {
              history[history_pos] = concat(m.lines);
              history_pos++;
              m.lines      = split(history[history_pos]);
              m.cursor_row = 1;
              if(m.cursor_col > m.lines[m.cursor_row - 1].size() + 1) { m.cursor_col = m.lines[m.cursor_row - 1].size() + 1; }
              if(m.lines.size() > scr.get_h()) { scr.set_h(m.lines.size()); }
            }
          }
          else
          {
            m.cursor_row++;
            if(m.cursor_col > m.lines[m.cursor_row - 1].size() + 1) { m.cursor_col = m.lines[m.cursor_row - 1].size() + 1; }
          }
          break;
        case Key::ENTER:
          not_complete = !iscomplete(concat(m.lines));
          if(not_complete) key = Key::ALT_ENTER;
          else
            break;
          CPP_TERMINAL_FALLTHROUGH;
        case Key::CTRL_N:
        case Key::ALT_ENTER:
        {
          std::string before        = m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
          std::string after         = m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
          m.lines[m.cursor_row - 1] = before;
          if(m.cursor_row < m.lines.size())
          {
            // Not at the bottom row, can't push back
            m.lines.insert(m.lines.begin() + m.cursor_row, after);
          }
          else { m.lines.push_back(after); }
          m.cursor_col = 1;
          m.cursor_row++;
          if(m.lines.size() > scr.get_h()) { scr.set_h(m.lines.size()); }
          break;
        }
        default: break;
      }
    }
    render(scr, m, cols);
    std::cout << scr.render(1, row, term_attached) << std::flush;
    if(row + (int)scr.get_h() - 1 > rows)
    {
      row = rows - ((int)scr.get_h() - 1);
      std::cout << scr.render(1, row, term_attached) << std::flush;
    }
  }
  std::string line_skips;
  for(std::size_t i = 0; i <= m.lines.size() - m.cursor_row; i++) { line_skips += "\n"; }
  std::cout << line_skips << std::flush;
  m_history.push_back(concat(m.lines));
  return concat(m.lines);
}
