#include "cpp-terminal/tty.hpp"

#ifdef _WIN32
  #include <io.h>
#else
  #include <unistd.h>
#endif

bool Term::is_stdin_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stdin));
#else
  return isatty(0);
#endif
}

bool Term::is_stdout_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stdout));
#else
  return isatty(1);
#endif
}

bool Term::is_stderr_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stderr));
#else
  return ::isatty(2);
#endif
}
