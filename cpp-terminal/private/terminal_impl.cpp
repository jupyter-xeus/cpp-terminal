/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/private/env.hpp"
#include "cpp-terminal/private/exception.hpp"
#include "cpp-terminal/private/file.hpp"
#include "cpp-terminal/terminal.hpp"

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
    const Term::Cursor cursor_before{Term::cursor_position()};
    Term::Private::out.write("\u001b%G");  // Try to activate UTF-8 (NOT warranty)
    const std::string  read{Term::Private::in.read()};
    const Term::Cursor cursor_after{Term::cursor_position()};
    const std::size_t  moved{cursor_after.column() - cursor_before.column()};
    std::string        rem;
    rem.reserve(moved * 3);
    for(std::size_t i = 0; i != moved; ++i) { rem += "\b \b"; }
    Term::Private::out.write(rem);
    enabled = 0 == moved;
  }
  else
  {
    // Does not return the original charset but, the default defined by standard ISO 8859-1 (ISO 2022);
    Term::Private::out.write("\u001b%@");
  }
#endif
}

void Term::Terminal::store_and_restore() noexcept
try
{
  static bool enabled{false};
#if defined(_WIN32)
  static DWORD originalOut{0};
  static DWORD originalIn{0};
  if(!enabled)
  {
    Term::Private::WindowsError().check_if(GetConsoleMode(Private::out.handle(), &originalOut) == 0).throw_exception("GetConsoleMode(Private::out.handle(), &originalOut)");
    Term::Private::WindowsError().check_if(GetConsoleMode(Private::in.handle(), &originalIn) == 0).throw_exception("GetConsoleMode(Private::in.handle(), &originalIn)");
    DWORD in{static_cast<DWORD>((originalIn & ~(ENABLE_QUICK_EDIT_MODE | setFocusEvents() | setMouseEvents())) | (ENABLE_EXTENDED_FLAGS))};
    DWORD out{originalOut};
    // Check if ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN can be activated, if not we are a legacy terminal.
    DWORD test = out;
    test |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if(!SetConsoleMode(Private::out.handle(), test)) { SetConsoleMode(Private::out.handle(), out); }
    else
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
    unsetMouseEvents();
    unsetFocusEvents();
    if(!Private::out.null()) { Term::Private::Errno().check_if(tcsetattr(Private::out.fd(), TCSAFLUSH, &orig_termios) == -1).throw_exception("tcsetattr() failed in destructor"); }
  }
#endif
}
catch(...)
{
  ExceptionHandler(Private::ExceptionDestination::StdErr);
}

std::size_t Term::Terminal::setMouseEvents()
{
#if defined(_WIN32)
  return static_cast<std::size_t>(ENABLE_MOUSE_INPUT);
#else
  return Term::Private::out.write("\u001b[?1002h\u001b[?1003h\u001b[?1006h");
#endif
}

std::size_t Term::Terminal::unsetMouseEvents()
{
#if defined(_WIN32)
  return static_cast<std::size_t>(ENABLE_MOUSE_INPUT);
#else
  return Term::Private::out.write("\u001b[?1006l\u001b[?1003l\u001b[?1002l");
#endif
}

std::size_t Term::Terminal::setFocusEvents()
{
#if defined(_WIN32)
  return static_cast<std::size_t>(ENABLE_WINDOW_INPUT);
#else
  return Term::Private::out.write("\u001b[?1004h");
#endif
}

std::size_t Term::Terminal::unsetFocusEvents()
{
#if defined(_WIN32)
  return static_cast<std::size_t>(ENABLE_WINDOW_INPUT);
#else
  return Term::Private::out.write("\u001b[?1004l");
#endif
}

void Term::Terminal::setMode() const
{
  static bool activated{false};
#if defined(_WIN32)
  static DWORD flags{0};
  if(!activated)
  {
    if(!Private::out.null())
      if(!GetConsoleMode(Private::in.handle(), &flags)) { throw Term::Private::WindowsException(GetLastError()); }
    activated = true;
    return;
  }
  DWORD send = flags;
  if(m_options.has(Option::Raw))
  {
    send &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    send |= (setFocusEvents() | setMouseEvents());
  }
  else if(m_options.has(Option::Cooked))
  {
    send |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    send &= ~(setFocusEvents() | setMouseEvents());
  }
  if(m_options.has(Option::NoSignalKeys)) { send &= ~ENABLE_PROCESSED_INPUT; }
  else if(m_options.has(Option::SignalKeys)) { send |= ENABLE_PROCESSED_INPUT; }
  if(!Private::out.null())
    if(!SetConsoleMode(Private::in.handle(), send)) { throw Term::Private::WindowsException(GetLastError()); }
#else
  if(!Private::out.null())
  {
    static ::termios raw = {};
    if(!activated)
    {
      Term::Private::Errno().check_if(tcgetattr(Private::out.fd(), &raw) == -1).throw_exception("tcgetattr(Private::out.fd(), &raw)");
      raw.c_cflag &= ~static_cast<std::size_t>(CSIZE | PARENB);
      raw.c_cflag |= CS8;
      raw.c_cc[VMIN]  = 1;
      raw.c_cc[VTIME] = 0;
      activated       = true;
      return;
    }
    ::termios send = raw;
    if(m_options.has(Option::Raw))
    {
      send.c_iflag &= ~static_cast<std::size_t>(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | INPCK);
      // This disables output post-processing, requiring explicit \r\n. We
      // keep it enabled, so that in C++, one can still just use std::endl
      // for EOL instead of "\r\n".
      //send.c_oflag &= ~static_cast<std::size_t>(OPOST);
      send.c_lflag &= ~static_cast<std::size_t>(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
      setMouseEvents();
      setFocusEvents();
    }
    else if(m_options.has(Option::Cooked))
    {
      send = raw;
      unsetMouseEvents();
      unsetFocusEvents();
    }
    if(m_options.has(Option::NoSignalKeys)) { send.c_lflag &= ~static_cast<std::size_t>(ISIG); }  //FIXME need others flags !
    else if(m_options.has(Option::NoSignalKeys)) { send.c_lflag |= ISIG; }
    Term::Private::Errno().check_if(tcsetattr(Private::out.fd(), TCSAFLUSH, &send) == -1).throw_exception("tcsetattr(Private::out.fd(), TCSAFLUSH, &send)");
  }
#endif
}
