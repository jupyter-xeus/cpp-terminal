#include "cpp-terminal/tty.hpp"

#ifdef _WIN32
  #include <io.h>
#else
  #include <unistd.h>
#endif

#include <cstdio>

bool is_a_tty(const FILE* fd)
{
#ifdef _WIN32
  return _isatty(_fileno(const_cast<FILE*>(fd)));
#else
  return isatty(fileno(const_cast<FILE*>(fd)));
#endif
}

bool Term::is_stdin_a_tty() { return is_a_tty(stdin); }

bool Term::is_stdout_a_tty() { return is_a_tty(stdout); }

bool Term::is_stderr_a_tty() { return is_a_tty(stderr); }
