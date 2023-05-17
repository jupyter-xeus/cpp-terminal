#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "exception.hpp"
#include "io.hpp"

#include <exception>
#include <iostream>

Term::Terminal::Terminal()
{
  setBadStateReturnCode();
  attachConsole();
  attachStreams();
  store_and_restore();
}

Term::Terminal::~Terminal()
{
  try
  {
    if(m_options.has(Option::ClearScreen)) clog << clear_buffer() << style(Style::RESET) << cursor_move(1, 1) << screen_load();
    if(m_options.has(Option::NoCursor)) clog << cursor_on();
    store_and_restore();
    // Starting from here the exceptions are not printed ! (Don't want to use cout here)
    detachStreams();
    detachConsole();
  }
  catch(const Term::Exception& e)
  {
    Term::terminal.operator<<("cpp-terminal has not been able to restore the terminal in a good state !") << std::endl;
    Term::terminal.operator<<("reason : ") << e.what() << std::endl;
    std::exit(m_badReturnCode);
  }
  catch(const std::exception& e)
  {
    Term::terminal.operator<<("cpp-terminal has not been able to restore the terminal in a good state !") << std::endl;
    Term::terminal.operator<<("reason : ") << e.what() << std::endl;
    std::exit(m_badReturnCode);
  }
  catch(...)
  {
    Term::terminal.operator<<("cpp-terminal has not been able to restore the terminal in a good state !") << std::endl;
    std::exit(m_badReturnCode);
  }
}

void Term::Terminal::setOptions() { applyOptions(); }

void Term::Terminal::applyOptions()
{
  if(m_options.has(Option::ClearScreen)) clog << screen_save() << clear_buffer() << style(Style::RESET) << cursor_move(1, 1);
  if(m_options.has(Option::NoCursor)) clog << cursor_off();
  if(m_options.has(Option::Raw)) setRawMode();
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
