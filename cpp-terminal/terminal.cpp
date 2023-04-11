#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"

#include <iostream>

Term::Terminal::Terminal()
{
  attachConsole();
  store_and_restore();
}

Term::Terminal::~Terminal()
{
  if(m_options.has(Option::ClearScreen))
  {
    // Fix consoles that ignore save_screen()
    std::cout << clear_buffer() << style(Style::RESET) << cursor_move(1, 1) << screen_load() << std::flush;
  }
  if(m_options.has(Option::NoCursor)) std::cout << cursor_on() << std::flush;
  // flush the output stream
  std::cout << std::flush;
  detachConsole();
  store_and_restore();
}

void Term::Terminal::setOptions(const std::vector<Term::Options::Option>& options)
{
  m_options = options;
  if(m_options.has(Option::Raw)) setRawMode();
  if(m_options.has(Option::ClearScreen))
  {
    // Fix consoles that ignore save_screen()
    std::cout << screen_save() << clear_buffer() << style(Style::RESET) << cursor_move(1, 1);
  }
  if(m_options.has(Option::NoCursor)) std::cout << cursor_off();
  std::cout << std::flush;
  std::cout << "DONE" << std::endl;
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
