#include "cpp-terminal/tty.hpp"

#ifdef _WIN32
 #include <io.h>
 #include <stdio.h>
#else
  #include <sys/ioctl.h>
#endif

namespace Term
{

bool Term::is_stdin_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stdin));
#else
  return isatty(STDIN_FILENO);
#endif
}

bool Term::is_stdout_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stdout));
#else
  return isatty(STDOUT_FILENO);
#endif
}

bool Term::is_stderr_a_tty()
{
#ifdef _WIN32
  return _isatty(_fileno(stderr));
#else
  return isatty(STDERR_FILENO);
#endif
}

}