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

std::string Term::Private::getenv(const std::string& env)
{
#ifdef _WIN32
  std::size_t requiredSize{0};
  getenv_s(&requiredSize, nullptr, 0, env.c_str());
  if(requiredSize == 0) return std::string();
  std::string ret;
  ret.reserve(requiredSize * sizeof(char));
  getenv_s(&requiredSize, &ret[0], requiredSize, env.c_str());
#else
  if(std::getenv(env.c_str()) != nullptr) return static_cast<std::string>(std::getenv(env.c_str()));
  else
    return std::string();
#endif
}

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

char Term::Private::read_raw_stdin()
{
  char c = getchar();
  if(c >= 0) { return c; }
  else if(c == EOF)
  {
    // In non-raw (blocking) mode this happens when the input file
    // ends. In such a case, return the End of Transmission (EOT)
    // character (Ctrl-D)
    return 0x04;
  }
  else { throw Term::Exception("getchar() failed"); }
}

bool Term::Private::read_raw(char* s)
{
  // do nothing when TTY is not connected
  if(!is_stdin_a_tty()) { return false; }
#ifdef _WIN32
  DWORD nread{0};
  GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &nread);
  if(nread >= 1)
  {
    char buf[1];
    if(!ReadFile(GetStdHandle(STD_INPUT_HANDLE), buf, 1, &nread, nullptr)) { Term::Exception("ReadFile() failed"); }
    if(nread == 1)
    {
      *s = buf[0];
      return true;
    }
    else { throw Term::Exception("kbhit() and ReadFile() inconsistent"); }
  }
  else { return false; }
#else
  ::ssize_t nread = ::read(0, s, 1);
  if(nread == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  return (nread == 1);
#endif
}
