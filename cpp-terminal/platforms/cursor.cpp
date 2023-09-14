#include "cpp-terminal/cursor.hpp"

#if defined(_WIN32)
  #include <windows.h>
#else
  #include "cpp-terminal/input.hpp"
#endif

#include "cpp-terminal/platforms/file.hpp"

Term::Cursor Term::cursor_position()
{
#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::out.handle(), &inf)) return Term::Cursor(static_cast<std::size_t>(inf.dwCursorPosition.Y + 1), static_cast<std::size_t>(inf.dwCursorPosition.X + 1));
  else
    return Term::Cursor(0, 0);
#else
  Term::Private::out.write(Term::cursor_position_report());
  do {
    std::string ret = Term::Private::in.read();
    if(ret[0] == '\033' && ret[1] == '[' && ret[ret.size() - 1] == 'R')
    {
      std::size_t found = ret.find(';', 2);
      if(found != std::string::npos) { return Cursor(std::stoi(ret.substr(2, found - 2)), std::stoi(ret.substr(found + 1, ret.size() - (found + 2)))); }
    }
  } while(true);
#endif
}
