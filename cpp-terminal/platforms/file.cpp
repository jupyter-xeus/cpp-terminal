#include "cpp-terminal/platforms/file.hpp"

#include <new>

#if defined(_WIN32)
  #include <fcntl.h>
  #include <io.h>
  #include <windows.h>
#else
  #include <cstdio>
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

Term::Private::FileHandler::FileHandler(const std::string& filename, const std::string& mode)
{
#if defined(_WIN32)
  m_handle = {CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
  if(m_handle == INVALID_HANDLE_VALUE)
  {
    m_handle = {CreateFile("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if(m_handle != INVALID_HANDLE_VALUE) m_isNull = true;
  }
  if(m_handle != INVALID_HANDLE_VALUE)
  {
    m_fd   = _open_osfhandle(reinterpret_cast<intptr_t>(m_handle), _O_RDWR);
    m_file = _fdopen(m_fd, mode.c_str());
  }
#else
  m_handle = {std::fopen(filename.c_str(), mode.c_str())};
  if(m_handle == nullptr)
  {
    m_handle = {std::fopen("/dev/null", mode.c_str())};
    m_isNull = true;
  }
  m_file = m_handle;
  m_fd   = fileno(m_handle);
#endif
  setvbuf(m_file, nullptr, _IONBF, 0);
}

Term::Private::FileHandler::~FileHandler()
{
#if defined(_WIN32)
  std::fclose(m_file);
#endif
}

bool Term::Private::FileHandler::isNull() { return m_isNull; }

FILE* Term::Private::FileHandler::file() { return m_file; }

int Term::Private::FileHandler::fd() { return m_fd; }

Term::Private::FileHandler::Handle Term::Private::FileHandler::getHandler() { return m_handle; }

int Term::Private::FileInitializer::m_counter = {0};

void Term::Private::FileInitializer::initialize()
{
  if(m_counter++ == 0)
  {
#if defined(_WIN32)
    new(&Term::Private::std_cin) FileHandler("CONIN$", "r");
    new(&Term::Private::std_cout) FileHandler("CONOUT$", "w");
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
