#include "cpp-terminal/cursor.hpp"

#if defined(_WIN32)
  #include "windows.h"
#else
  #include "cpp-terminal/input.hpp"
  #include "cpp-terminal/terminal.hpp"
#endif

Term::Cursor Term::cursor_position()
{
#if defined(_WIN32)
  Term::Cursor               ret;
  HANDLE                     hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(hConOut, &inf)) { ret = {static_cast<std::size_t>(inf.dwCursorPosition.Y + 1), static_cast<std::size_t>(inf.dwCursorPosition.X + 1)}; }
  CloseHandle(hConOut);
  return ret;
#else
  Term::terminal << Term::cursor_position_report();
  Term::Cursor c;
  while((c = Platform::read_raw()).empty()) continue;
  return c;
#endif
}
