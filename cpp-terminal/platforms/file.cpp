#include "cpp-terminal/platforms/file.hpp"

#include <new>

#if defined(_WIN32)
  #include <windows.h>
typedef void* Handle;
#else
  #include <cstdio>
typedef FILE* Handle;
#endif

namespace Term
{

namespace Private
{
static char                 stdin_buf[sizeof(Term::Private::FileHandler)];
Term::Private::FileHandler& std_cin = reinterpret_cast<Term::Private::FileHandler&>(stdin_buf);
static char                 stdout_buf[sizeof(Term::Private::FileHandler)];
Term::Private::FileHandler& std_cout = reinterpret_cast<Term::Private::FileHandler&>(stdout_buf);
}  // namespace Private

}  // namespace Term

Term::Private::FileHandler::FileHandler(const char* filename, const char* mode)
{
#if defined(_WIN32)
  m_file = {CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  if(m_file == INVALID_HANDLE_VALUE)
  {
    m_file   = {CreateFile("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    m_isNull = true;
  }
#else
  m_file = {std::fopen(filename, mode)};
  if(m_file == nullptr)
  {
    m_file   = {std::fopen("/dev/null", mode)};
    m_isNull = true;
  }
#endif
}

Term::Private::FileHandler::~FileHandler()
{
#if defined(_WIN32)
  CloseHandle(reinterpret_cast<HANDLE>(m_file));
#else
  std::fclose(reinterpret_cast<FILE*>(m_file));
#endif
}

bool Term::Private::FileHandler::isNull() { return m_isNull; }

Term::Private::consoleFileHandler Term::Private::FileHandler::getHandler()
{
#if defined(_WIN32)
  return m_file;
#else
  return fileno(reinterpret_cast<FILE*>(m_file));
#endif
}

int Term::Private::FileInitializer::m_counter = {0};

void Term::Private::FileInitializer::initialize()
{
  if(m_counter++ == 0)
  {
#if defined(_WIN32)
    new(&Term::Private::std_cin) FileHandler("CONIN$", "");
    new(&Term::Private::std_cout) FileHandler("CONOUT$", "");
#else
    new(&Term::Private::std_cin) FileHandler("/dev/tty", "r");
    new(&Term::Private::std_cout) FileHandler("/dev/tty", "w");
#endif
  }
}

Term::Private::FileInitializer::FileInitializer() { initialize(); }

Term::Private::FileInitializer::~FileInitializer()
{
  if(--m_counter == 0)
  {
    (&Term::Private::std_cin)->~FileHandler();
    (&Term::Private::std_cout)->~FileHandler();
  }
}
