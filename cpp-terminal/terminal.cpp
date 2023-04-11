#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "options.hpp"

Term::Terminal::Terminal()
{
  attachConsole();
  attachStreams();
  store_and_restore();
}

Term::Terminal::~Terminal()
{
  if(m_options.has(Option::ClearScreen)) clog << clear_buffer() << style(Style::RESET) << cursor_move(1, 1) << screen_load();
  if(m_options.has(Option::NoCursor)) clog << cursor_on();
  store_and_restore();
  detachStreams();
  detachConsole();
}

void Term::Terminal::setOptions(const std::vector<Term::Options::Option>& options)
{
  m_options = Options(options);
  if(m_options.has(Option::ClearScreen)) clog << screen_save() << clear_buffer() << style(Style::RESET) << cursor_move(1, 1);
  if(m_options.has(Option::NoCursor)) clog << cursor_off();
  if(m_options.has(Option::Raw)) setRawMode();
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
