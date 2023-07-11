#include "cpp-terminal/platforms/file.hpp"

#include <new>

#if defined(_WIN32)
  #include <io.h>
  #include <windows.h>
#else
  #include <cstdio>
  #include <unistd.h>
#endif

#include <fcntl.h>

namespace Term
{

namespace Private
{
static char                       stdin_buf[sizeof(Term::Private::InputFileHandler)];
Term::Private::InputFileHandler&  in = reinterpret_cast<Term::Private::InputFileHandler&>(stdin_buf);
static char                       stdout_buf[sizeof(Term::Private::OutputFileHandler)];
Term::Private::OutputFileHandler& out = reinterpret_cast<Term::Private::OutputFileHandler&>(stdout_buf);
}  // namespace Private

}  // namespace Term

Term::Private::FileHandler::FileHandler(const std::string& filename, const std::string& mode)
{
#if defined(_WIN32)
  m_handle = {CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
  if(m_handle == INVALID_HANDLE_VALUE)
  {
    m_handle = {CreateFile("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if(m_handle != INVALID_HANDLE_VALUE) m_null = true;
  }
  if(m_handle != INVALID_HANDLE_VALUE)
  {
    m_fd   = _open_osfhandle(reinterpret_cast<intptr_t>(m_handle), _O_RDWR);
    m_file = _fdopen(m_fd, mode.c_str());
  }
#else
  std::size_t flag{O_ASYNC | O_DSYNC | O_NOCTTY | O_SYNC};
  if(mode.find('r') != std::string::npos) flag |= O_RDONLY;
  else if(mode.find('w') != std::string::npos)
    flag |= O_WRONLY;
  else
    flag |= O_RDWR;
  m_fd = {::open(filename.c_str(), flag)};
  if(m_fd == -1)
  {
    m_fd = {::open("/dev/null", flag)};
    if(m_fd != -1) m_null = true;
  }
  if(m_fd != -1)
  {
    m_file   = fdopen(m_fd, mode.c_str());
    m_handle = m_file;
  }
#endif
  setvbuf(m_file, nullptr, _IONBF, 0);
}

Term::Private::FileHandler::~FileHandler()
{
#if defined(_WIN32)
  std::fclose(m_file);
#endif
}

bool Term::Private::FileHandler::null() const { return m_null; }

FILE* Term::Private::FileHandler::file() { return m_file; }

int Term::Private::FileHandler::fd() const { return m_fd; }

Term::Private::FileHandler::Handle Term::Private::FileHandler::handle() { return m_handle; }

int Term::Private::FileInitializer::m_counter = {0};

void Term::Private::FileInitializer::init()
{
  if(m_counter++ == 0)
  {
#if defined(_WIN32)
    new(&Term::Private::in) InputFileHandler("CONIN$");
    new(&Term::Private::out) OutputFileHandler("CONOUT$");
#else
    new(&Term::Private::in) InputFileHandler("/dev/tty");
    new(&Term::Private::out) OutputFileHandler("/dev/tty");
#endif
  }
}

Term::Private::FileInitializer::~FileInitializer()
{
  if(--m_counter == 0)
  {
    (&Term::Private::in)->~InputFileHandler();
    (&Term::Private::out)->~OutputFileHandler();
  }
}

int Term::Private::OutputFileHandler::write(const std::string& str)
{
#if defined(_WIN32)
  DWORD dwCount;
  if(WriteConsole(handle(), &str[0], str.size(), &dwCount, nullptr) == 0) return -1;
#else
  return ::write(fd(), &str[0], str.size());
#endif
}
