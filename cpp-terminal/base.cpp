#include "cpp-terminal/base.hpp"

#include "cpp-terminal/input.hpp"
#include "cpp-terminal/platforms/platform.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <utility>

std::string Term::style(Term::Style style) { return "\033[" + std::to_string((std::uint8_t)style) + 'm'; }

std::pair<std::size_t, std::size_t> Term::get_size()
{
  return Private::get_term_size();  // function uses platform dependent code
}

bool Term::stdin_connected() { return Term::is_stdin_a_tty(); }

bool Term::stdout_connected() { return Term::is_stdout_a_tty(); }

std::string Term::cursor_off() { return "\x1b[?25l"; }

std::string Term::cursor_on() { return "\x1b[?25h"; }

std::string Term::clear_screen() { return "\033[2J"; }
std::string Term::clear_buffer() { return "\033[3J"; }

std::string Term::cursor_move(std::size_t row, std::size_t column) { return "\033[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }

std::string Term::cursor_up(std::size_t rows) { return "\033[" + std::to_string(rows) + 'A'; }

std::string Term::cursor_down(std::size_t rows) { return "\033[" + std::to_string(rows) + 'B'; }

std::string Term::cursor_right(std::size_t columns) { return "\033[" + std::to_string(columns) + 'C'; }

std::string Term::cursor_left(std::size_t columns) { return "\033[" + std::to_string(columns) + 'D'; }

std::pair<std::size_t, std::size_t> Term::cursor_position()
{
  // write cursor position report
  std::cout << cursor_position_report() << std::flush;
  // read input buffer
  std::string buf;
  char        c{'\0'};
  do {
    while(!Platform::read_raw(&c))
      ;
    buf.push_back(c);
  } while(c != 'R');

  bool        found{false};
  std::size_t row{0};
  std::size_t column{0};
  for(std::size_t i = 2; i < buf.size(); i++)
  {
    if(buf[i] == ';') found = true;
    else if(found == false && buf[i] >= '0' && buf[i] <= '9')
      row = row * 10 + (buf[i] - '0');
    else if(found == true && buf[i] >= '0' && buf[i] <= '9')
      column = column * 10 + (buf[i] - '0');
  }
  return std::pair<std::size_t, std::size_t>(row, column);
}

std::string Term::cursor_position_report() { return "\x1b[6n"; }

std::string Term::clear_eol() { return "\033[K"; }

std::string Term::screen_save()
{
  return "\0337\033[?1049h";  // save current cursor position, save screen
}
std::string Term::screen_load()
{
  return "\033[?1049l\0338";  // restores screen, restore current cursor position
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }
