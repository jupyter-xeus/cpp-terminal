#include "cpp-terminal/screen.hpp"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <fcntl.h>
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

Term::Screen Term::screen_size()
{
#ifdef _WIN32
  Term::Screen               ret;
  HANDLE                     hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(hConOut, &inf)) { ret = {static_cast<std::size_t>(inf.srWindow.Bottom - inf.srWindow.Top + 1), static_cast<std::size_t>(inf.srWindow.Right - inf.srWindow.Left + 1)}; }
  CloseHandle(hConOut);
  return ret;
#else
  Term::Screen   ret;
  struct winsize window
  {
    0, 0, 0, 0
  };
  int fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
  if(ioctl(fd, TIOCGWINSZ, &window) != -1) ret = {window.ws_row, window.ws_col};
  close(fd);
  return ret;
#endif
}
