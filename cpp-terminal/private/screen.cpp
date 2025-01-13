/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/screen.hpp"

#ifdef _WIN32
  #pragma warning(push)
  #pragma warning(disable : 4668)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #pragma warning(pop)
#else
  #include <sys/ioctl.h>
#endif

#include "cpp-terminal/private/file.hpp"

Term::Screen Term::screen_size()
{
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::out.handle(), &inf)) return Term::Screen({Term::Rows(inf.srWindow.Bottom - inf.srWindow.Top + 1), Term::Columns(inf.srWindow.Right - inf.srWindow.Left + 1)});
  return {};
#else
  struct winsize window{0, 0, 0, 0};
  if(ioctl(Private::out.fd(), TIOCGWINSZ, &window) != -1) return Term::Screen({Term::Rows(window.ws_row), Term::Columns(window.ws_col)});
  return {};
#endif
}
