#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"

#include <exception>

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
    if(m_options.has(Option::ClearScreen)) Term::Private::out.write(clear_buffer() + style(Style::RESET) + cursor_move(1, 1) + screen_load());
    if(m_options.has(Option::NoCursor)) Term::Private::out.write(cursor_on());
    store_and_restore();
    // Starting from here the exceptions are not printed ! (Don't want to use cout here)
    detachStreams();
    detachConsole();
  }
  catch(const Term::Exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\nreason : " + std::string(e.what()) + '\n');
    std::exit(m_badReturnCode);
  }
  catch(const std::exception& e)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\nreason : " + std::string(e.what()) + '\n');
    std::exit(m_badReturnCode);
  }
  catch(...)
  {
    Term::Private::out.write("cpp-terminal has not been able to restore the terminal in a good state !\n");
    std::exit(m_badReturnCode);
  }
}

void Term::Terminal::setOptions() { applyOptions(); }

void Term::Terminal::applyOptions()
{
  if(m_options.has(Option::ClearScreen)) Term::Private::out.write(screen_save() + clear_buffer() + style(Style::RESET) + cursor_move(1, 1));
  if(m_options.has(Option::NoCursor)) Term::Private::out.write(cursor_off());
  if(m_options.has(Option::Raw)) setRawMode();
}

std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }

std::string Term::clear_buffer() { return "\033[3J"; }
