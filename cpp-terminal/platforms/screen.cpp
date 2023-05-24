#include "cpp-terminal/screen.hpp"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/ioctl.h>
#endif

#include "cpp-terminal/platforms/file.hpp"

Term::Screen Term::screen_size()
{
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO inf;
  if(GetConsoleScreenBufferInfo(Private::std_cout.getHandler(), &inf)) return Term::Screen(static_cast<std::size_t>(inf.srWindow.Bottom - inf.srWindow.Top + 1), static_cast<std::size_t>(inf.srWindow.Right - inf.srWindow.Left + 1));
  return Term::Screen();
#else
  Term::Screen   ret;
  struct winsize window
  {
    0, 0, 0, 0
  };
  if(ioctl(Private::std_cout.getHandler(), TIOCGWINSZ, &window) != -1) ret = {window.ws_row, window.ws_col};
  return ret;
#endif
}
