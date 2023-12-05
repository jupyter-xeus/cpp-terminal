/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"

#if defined(_WIN32)
  #include <windows.h>
#endif

#include "cpp-terminal/private/file.hpp"

Term::Cursor Term::cursor_position()
{
  static const Term::Private::FileInitializer files_init;
  if(Term::Private::in.null()) { return {}; }
#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::out.handle(), &inf)) return {static_cast<std::size_t>(inf.dwCursorPosition.Y + 1), static_cast<std::size_t>(inf.dwCursorPosition.X + 1)};
  return {};
#else
  std::string ret{Term::Private::ask(Term::cursor_position_report())};
  if(('\u001b' == ret[0]) && ('[' == ret[1]) && ('R' == ret[ret.size() - 1]))
  {
    const std::size_t found{ret.find(';', 2)};
    if(found != std::string::npos) { return {std::stoi(ret.substr(2, found - 2)), std::stoi(ret.substr(found + 1, ret.size() - (found + 2)))}; }
    return {};
  }
  return {};
#endif
}
