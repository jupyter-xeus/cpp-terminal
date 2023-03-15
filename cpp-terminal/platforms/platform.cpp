#ifdef _WIN32
  #include <windows.h>
#else
  #include <cerrno>
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/platforms/platform.hpp"
#include "cpp-terminal/tty.hpp"

std::pair<std::size_t, std::size_t> Term::Private::get_term_size()
{
#ifdef _WIN32
  if(GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw Term::Exception("GetStdHandle(STD_OUTPUT_HANDLE) failed"); }
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf))
  {
    std::size_t cols = inf.srWindow.Right - inf.srWindow.Left + 1;
    std::size_t rows = inf.srWindow.Bottom - inf.srWindow.Top + 1;
    return {rows, cols};
  }
  else
  {
    // This happens when we are not connected to a terminal
    throw Term::Exception("Couldn't get terminal size. Is it connected to a TTY?");
  }
#else
  struct winsize ws
  {
  };
  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
  {
    // This happens when we are not connected to a terminal
    throw Term::Exception("Couldn't get terminal size. Is it connected to a TTY?");
  }
  else { return std::pair<std::size_t, std::size_t>{ws.ws_row, ws.ws_col}; }
#endif
}
