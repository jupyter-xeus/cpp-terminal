/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <termios.h>
#endif

#include "cpp-terminal/platforms/file.hpp"

Term::Cursor Term::cursor_position()
{
  if(Term::Private::in.null()) return Term::Cursor();
#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::out.handle(), &inf)) return Term::Cursor(static_cast<std::size_t>(inf.dwCursorPosition.Y + 1), static_cast<std::size_t>(inf.dwCursorPosition.X + 1));
  else
    return Term::Cursor(0, 0);
#else
  std::string ret;
  std::size_t nread{0};
  Term::Private::in.lockIO();
  // Hack to be sure we can do this all the time "Cooked" or "Raw" mode
  ::termios actual;
  if(!Private::out.null())
    if(tcgetattr(Private::out.fd(), &actual) == -1) return Term::Cursor();
  ::termios raw = actual;
  // Put terminal in raw mode
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  // This disables output post-processing, requiring explicit \r\n. We
  // keep it enabled, so that in C++, one can still just use std::endl
  // for EOL instead of "\r\n".
  // raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  raw.c_lflag &= ~ISIG;
  raw.c_cc[VMIN]  = 1;
  raw.c_cc[VTIME] = 0;
  if(!Private::out.null()) tcsetattr(Private::out.fd(), TCSAFLUSH, &raw);
  Term::Private::out.write(Term::cursor_position_report());
  while(nread == 0) { ::ioctl(Private::in.fd(), FIONREAD, &nread); }
  ret = Term::Private::in.read();
  tcsetattr(Private::out.fd(), TCSAFLUSH, &actual);
  Term::Private::in.unlockIO();
  try
  {
    if(ret[0] == '\033' && ret[1] == '[' && ret[ret.size() - 1] == 'R')
    {
      std::size_t found = ret.find(';', 2);
      if(found != std::string::npos) { return Cursor(std::stoi(ret.substr(2, found - 2)), std::stoi(ret.substr(found + 1, ret.size() - (found + 2)))); }
      else
        return Term::Cursor();
    }
    return Term::Cursor();
  }
  catch(...)
  {
    return Term::Cursor();
  }
#endif
}
