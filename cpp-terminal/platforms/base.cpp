#include "cpp-terminal/base.hpp"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <cerrno>
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

std::pair<std::size_t, std::size_t> Term::get_size()
{
#ifdef _WIN32
  if(GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw Term::Exception("GetStdHandle(STD_OUTPUT_HANDLE) failed"); }
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf)) return {inf.srWindow.Bottom - inf.srWindow.Top + 1, inf.srWindow.Right - inf.srWindow.Left + 1};
  else
    return {0, 0};
#else
  struct winsize ws
  {
  };
  ws.ws_row    = 0;
  ws.ws_col    = 0;
  ws.ws_xpixel = 0;
  ws.ws_ypixel = 0;
  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) return {0, 0};
  else
    return std::pair<std::size_t, std::size_t>{ws.ws_row, ws.ws_col};
#endif
}
