/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/platforms/env.hpp"
#include "cpp-terminal/platforms/file.hpp"

#ifdef _WIN32
  #include <io.h>
  #include <windows.h>
  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
  #ifndef DISABLE_NEWLINE_AUTO_RETURN
    #define DISABLE_NEWLINE_AUTO_RETURN 0x0008
  #endif
  #ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
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
    out_code_page = GetConsoleOutputCP();
    if(out_code_page == 0) throw Term::Exception("GetConsoleOutputCP() failed");
    if(!SetConsoleOutputCP(CP_UTF8)) throw Term::Exception("SetConsoleOutputCP(CP_UTF8) failed");
    in_code_page = GetConsoleCP();
    if(out_code_page == 0) throw Term::Exception("GetConsoleCP() failed");
    if(!SetConsoleCP(CP_UTF8)) throw Term::Exception("SetConsoleCP(CP_UTF8) failed");
    enabled = true;
  }
  else
  {
    if(!SetConsoleOutputCP(out_code_page)) throw Term::Exception("SetConsoleOutputCP(out_code_page) failed");
    if(!SetConsoleCP(in_code_page)) throw Term::Exception("SetConsoleCP(in_code_page) failed");
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

void Term::Terminal::store_and_restore()
{
  static bool enabled{false};
#ifdef _WIN32
  static DWORD dwOriginalOutMode{0};
  static DWORD dwOriginalInMode{0};
  if(!enabled)
  {
    if(GetConsoleMode(Private::out.handle(), &dwOriginalOutMode) == 0) { throw Term::Exception("GetConsoleMode() failed"); }
    if(GetConsoleMode(Private::in.handle(), &dwOriginalInMode) == 0) { throw Term::Exception("GetConsoleMode() failed"); }
    DWORD in{(dwOriginalInMode & ~ENABLE_QUICK_EDIT_MODE) | (ENABLE_EXTENDED_FLAGS | activateFocusEvents() | activateMouseEvents())};
    DWORD out{dwOriginalOutMode};
    if(!m_terminfo.isLegacy())
    {
      out |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
      in |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    }
    if(!SetConsoleMode(Private::out.handle(), out)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    if(!SetConsoleMode(Private::in.handle(), in)) { throw Term::Exception("SetConsoleMode() failed"); }
    enabled = true;
  }
  else
  {
    if(!SetConsoleMode(Private::out.handle(), dwOriginalOutMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    if(!SetConsoleMode(Private::in.handle(), dwOriginalInMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    enabled = false;
  }
#else
  static termios orig_termios;
  if(!enabled)
  {
    if(!Private::out.null())
      if(tcgetattr(Private::out.fd(), &orig_termios) == -1) { throw Term::Exception("tcgetattr() failed"); }
    termios term = orig_termios;
    term.c_cflag &= ~PARENB;  // Clear parity bit, disabling parity (most common)
    term.c_cflag &= ~CSTOPB;  // Clear stop field, only one stop bit used in communication (most common)
    term.c_cflag &= ~CSIZE;   // Clear all the size bits, then use one of the statements below
    term.c_cflag |= CS8;      // 8 bits per byte (most common)
    if(!Private::out.null())
      if(tcsetattr(Private::out.fd(), TCSAFLUSH, &term) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
    enabled = true;
  }
  else
  {
    if(!Private::out.null())
      if(tcsetattr(Private::out.fd(), TCSAFLUSH, &orig_termios) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
    enabled = false;
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
  return Term::Private::out.write("\033[?1003l\033[?1006l");
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

void Term::Terminal::setRawMode()
{
#if defined(_WIN32)
  DWORD flags{0};
  if(!GetConsoleMode(Private::in.handle(), &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_options.has(Option::NoSignalKeys)) { flags &= ~ENABLE_PROCESSED_INPUT; }
  flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  if(!SetConsoleMode(Private::in.handle(), flags)) { throw Term::Exception("SetConsoleMode() failed"); }
#else
  if(!Private::out.null())
  {
    ::termios raw;
    if(tcgetattr(Private::out.fd(), &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
    // Put terminal in raw mode
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // This disables output post-processing, requiring explicit \r\n. We
    // keep it enabled, so that in C++, one can still just use std::endl
    // for EOL instead of "\r\n".
    // raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    if(m_options.has(Option::NoSignalKeys)) { raw.c_lflag &= ~ISIG; }
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    if(tcsetattr(Private::out.fd(), TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
    activateMouseEvents();
    activateFocusEvents();
  }
#endif
}
