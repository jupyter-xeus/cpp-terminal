/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/platforms/env.hpp"
#include "cpp-terminal/platforms/exception.hpp"
#include "cpp-terminal/platforms/file.hpp"

#if defined(_WIN32)
  #include <io.h>
  #include <windows.h>
  #if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
  #if !defined(DISABLE_NEWLINE_AUTO_RETURN)
    #define DISABLE_NEWLINE_AUTO_RETURN 0x0008
  #endif
  #if !defined(ENABLE_VIRTUAL_TERMINAL_INPUT)
    #define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
  #endif
#else
  #include <termios.h>
#endif

void Term::Terminal::set_unset_utf8()
{
  static bool enabled{false};
#if defined(_WIN32)
  static UINT out_code_page{0};
  static UINT in_code_page{0};
  if(!enabled)
  {
    if((out_code_page = GetConsoleOutputCP()) == 0) throw Term::Private::WindowsException(GetLastError(), "GetConsoleOutputCP()");
    if(!SetConsoleOutputCP(CP_UTF8)) throw Term::Private::WindowsException(GetLastError(), "SetConsoleOutputCP(CP_UTF8)");
    if((in_code_page = GetConsoleCP()) == 0) throw Term::Private::WindowsException(GetLastError(), "GetConsoleCP()");
    if(!SetConsoleCP(CP_UTF8)) throw Term::Private::WindowsException(GetLastError(), "SetConsoleCP(CP_UTF8)");
    enabled = true;
  }
  else
  {
    if(!SetConsoleOutputCP(out_code_page)) throw Term::Private::WindowsException(GetLastError(), "SetConsoleOutputCP(out_code_page)");
    if(!SetConsoleCP(in_code_page)) throw Term::Private::WindowsException(GetLastError(), "SetConsoleCP(in_code_page)");
  }
#else
  if(!enabled)
  {
    if(m_terminfo.getName() != "Apple_Terminal") Term::Private::out.write("\033%G");
    enabled = true;
  }
  else
  {
    // Does not return the original charset but, the default defined by standard ISO 8859-1 (ISO 2022);
    if(m_terminfo.getName() != "Apple_Terminal") Term::Private::out.write("\033%@");
  }
#endif
}

///
///@brief Store and restore the default state of the terminal. Configure the default mode for cpp-terminal.
///
void Term::Terminal::store_and_restore()
{
  static bool enabled{false};
#if defined(_WIN32)
  static DWORD originalOut{0};
  static DWORD originalIn{0};
  if(!enabled)
  {
    Term::Private::WindowsError().check_if(GetConsoleMode(Private::out.handle(), &originalOut) == 0).throw_exception("GetConsoleMode(Private::out.handle(), &originalOut)");
    Term::Private::WindowsError().check_if(GetConsoleMode(Private::in.handle(), &originalIn) == 0).throw_exception("GetConsoleMode(Private::in.handle(), &originalIn)");
    DWORD in{(originalIn & ~ENABLE_QUICK_EDIT_MODE) | (ENABLE_EXTENDED_FLAGS | activateFocusEvents() | activateMouseEvents())};
    DWORD out{originalOut};
    if(!m_terminfo.isLegacy())
    {
      out |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
      in |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    }
    if(!SetConsoleMode(Private::out.handle(), out)) { throw Term::Private::WindowsException(GetLastError(), "SetConsoleMode(Private::out.handle()"); }
    if(!SetConsoleMode(Private::in.handle(), in)) { throw Term::Private::WindowsException(GetLastError(), "SetConsoleMode(Private::in.handle(), in)"); }
    enabled = true;
  }
  else
  {
    if(!SetConsoleMode(Private::out.handle(), originalOut)) { throw Term::Private::WindowsException(GetLastError(), "SetConsoleMode(Private::out.handle(), originalOut)"); }
    if(!SetConsoleMode(Private::in.handle(), originalIn)) { throw Term::Private::WindowsException(GetLastError(), "SetConsoleMode(Private::in.handle(), originalIn)"); }
  }
#else
  static termios orig_termios;
  if(!enabled)
  {
    if(!Private::out.null()) { Term::Private::Errno().check_if(tcgetattr(Private::out.fd(), &orig_termios) == -1).throw_exception("tcgetattr() failed"); }
    enabled = true;
  }
  else
  {
    desactivateMouseEvents();
    desactivateFocusEvents();
    if(!Private::out.null()) { Term::Private::Errno().check_if(tcsetattr(Private::out.fd(), TCSAFLUSH, &orig_termios) == -1).throw_exception("tcsetattr() failed in destructor"); }
  }
#endif
}

int Term::Terminal::activateMouseEvents()
{
#if defined(_WIN32)
  return ENABLE_MOUSE_INPUT;
#else
  return Term::Private::out.write("\033[?1002h\033[?1003h\033[?1006h");
#endif
}

int Term::Terminal::desactivateMouseEvents()
{
#if defined(_WIN32)
  return ENABLE_MOUSE_INPUT;
#else
  return Term::Private::out.write("\033[?1006l\033[?1003l\033[?1002l");
#endif
}

int Term::Terminal::activateFocusEvents()
{
#if defined(_WIN32)
  return ENABLE_WINDOW_INPUT;
#else
  return Term::Private::out.write("\033[?1004h");
#endif
}

int Term::Terminal::desactivateFocusEvents()
{
#if defined(_WIN32)
  return ENABLE_WINDOW_INPUT;
#else
  return Term::Private::out.write("\033[?1004l");
#endif
}

void Term::Terminal::setBadStateReturnCode()
{
  std::pair<bool, std::string> returnCode{Private::getenv("CPP_TERMINAL_BADSTATE")};
  try
  {
    if(returnCode.first && stoi(returnCode.second) != EXIT_SUCCESS) m_badReturnCode = static_cast<std::uint8_t>(stoi(returnCode.second));
  }
  catch(...)
  {
    m_badReturnCode = EXIT_FAILURE;
  }
}

///
///@brief Set mode raw/cooked.
///First call is to save the good state set-up by cpp-terminal.
///
void Term::Terminal::setMode()
{
  static bool activated{false};
#if defined(_WIN32)
  static DWORD flags{0};
  if(!activated)
  {
    if(!Private::out.null())
      if(!GetConsoleMode(Private::in.handle(), &flags)) { throw Term::Private::WindowsException(GetLastError()); }
    activated = true;
  }
  DWORD send = flags;
  if(m_options.has(Option::Raw)) { send &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT); }
  else if(m_options.has(Option::Cooked)) { send |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT); }
  if(m_options.has(Option::NoSignalKeys)) { send &= ~ENABLE_PROCESSED_INPUT; }
  else if(m_options.has(Option::SignalKeys)) { send |= ENABLE_PROCESSED_INPUT; }
  if(!Private::out.null())
    if(!SetConsoleMode(Private::in.handle(), send)) { throw Term::Private::WindowsException(GetLastError()); }
#else
  if(!Private::out.null())
  {
    static ::termios raw;
    if(!activated)
    {
      if(tcgetattr(Private::out.fd(), &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
      activated = true;
    }
    ::termios send = raw;
    if(m_options.has(Option::Raw))
    {
      // Put terminal in raw mode
      send.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
      // This disables output post-processing, requiring explicit \r\n. We
      // keep it enabled, so that in C++, one can still just use std::endl
      // for EOL instead of "\r\n".
      // raw.c_oflag &= ~(OPOST);
      send.c_lflag &= ~(ECHO | ICANON | IEXTEN);
      send.c_cc[VMIN]  = 1;
      send.c_cc[VTIME] = 0;
      activateMouseEvents();
      activateFocusEvents();
    }
    else if(m_options.has(Option::Cooked))
    {
      send = raw;
      desactivateMouseEvents();
      desactivateFocusEvents();
    }
    if(m_options.has(Option::NoSignalKeys)) { send.c_lflag &= ~ISIG; }  //FIXME need others flags !
    else if(m_options.has(Option::NoSignalKeys)) { send.c_lflag |= ISIG; }
    if(tcsetattr(Private::out.fd(), TCSAFLUSH, &send) == -1) { throw Term::Exception("tcsetattr() failed"); }
  }
#endif
}
