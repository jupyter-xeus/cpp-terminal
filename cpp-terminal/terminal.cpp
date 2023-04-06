#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/base.hpp"

#include <iostream>

Term::Terminal::Terminal(const std::vector<Term::Options::Option>& options) : m_options(options)
{
  attachConsole();
  store_and_restore();
  setRawMode();
  if(m_options.has(Option::ClearScreen))
  {
    // Fix consoles that ignore save_screen()
    std::cout << screen_save() << clear_buffer() << style(Style::RESET) << cursor_move(1, 1);
  }
  if(m_options.has(Option::NoCursor)) std::cout << cursor_off();
  std::cout << std::flush;
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
