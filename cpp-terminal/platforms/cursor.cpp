#include "cpp-terminal/cursor.hpp"

#if defined(_WIN32)
  #include "cpp-terminal/platforms/file.hpp"
  #include "windows.h"
#else
  #include "cpp-terminal/input.hpp"
  #include "cpp-terminal/terminal.hpp"
#endif

Term::Cursor Term::cursor_position()
{
#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::std_cout.getHandler(), &inf)) return Term::Cursor(inf.dwCursorPosition.Y + 1, inf.dwCursorPosition.X + 1);
  else
    return Term::Cursor(0, 0);
#else
  Term::terminal << Term::cursor_position_report();
  Term::Cursor c;
  while((c = Platform::read_raw()).empty()) continue;
  return c;
#endif
}
