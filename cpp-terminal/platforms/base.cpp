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
  static HANDLE                       hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  CONSOLE_SCREEN_BUFFER_INFO          inf;
  std::pair<std::size_t, std::size_t> ret{0, 0};
  if(GetConsoleScreenBufferInfo(hConOut, &inf)) { ret = {inf.srWindow.Bottom - inf.srWindow.Top + 1, inf.srWindow.Right - inf.srWindow.Left + 1}; }
  CloseHandle(hConOut);
  return ret;
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
