/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/file.hpp"

#include <new>

#if defined(_WIN32)
  #include <io.h>
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/exception.hpp"

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

Term::Private::FileHandler::FileHandler(std::recursive_mutex& mutex, const std::string& filename, const std::string& mode) : m_mutex(mutex)
{
#if defined(_WIN32)
  m_handle = CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if(m_handle == INVALID_HANDLE_VALUE)
  {
    m_handle = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(m_handle != INVALID_HANDLE_VALUE) m_null = true;
  }
  if(m_handle != INVALID_HANDLE_VALUE)
  {
    m_fd   = _open_osfhandle(reinterpret_cast<intptr_t>(m_handle), _O_RDWR);
    m_file = _fdopen(m_fd, mode.c_str());
  }
#else
  std::size_t flag{O_ASYNC | O_DSYNC | O_NOCTTY | O_SYNC | O_NDELAY};
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
  std::fflush(m_file);
  std::fclose(m_file);
}

bool Term::Private::FileHandler::null() const { return m_null; }

FILE* Term::Private::FileHandler::file() { return m_file; }

int Term::Private::FileHandler::fd() const { return m_fd; }

Term::Private::FileHandler::Handle Term::Private::FileHandler::handle() { return m_handle; }

int Term::Private::FileInitializer::m_counter = {0};

void Term::Private::FileInitializer::attachConsole()
{
#if defined(_WIN32)
  BOOL attached{AttachConsole(ATTACH_PARENT_PROCESS)};
  // Iw we don't have console try to create one
  if(GetLastError() == ERROR_INVALID_HANDLE && !attached)
  {
    if(AllocConsole())
    {
      hadToAttachConsole = true;
      AttachConsole(GetCurrentProcessId());
    }
  }
  FILE* fDummy{nullptr};
  if(_fileno(stdout) < 0 || _get_osfhandle(_fileno(stdout)) < 0) freopen_s(&fDummy, "CONOUT$", "w", stdout);
  if(_fileno(stderr) < 0 || _get_osfhandle(_fileno(stderr)) < 0) freopen_s(&fDummy, "CONOUT$", "w", stderr);
  if(_fileno(stdin) < 0 || _get_osfhandle(_fileno(stdin)) < 0) freopen_s(&fDummy, "CONIN$", "r", stdin);
#endif
  setvbuf(stdin, nullptr, _IOLBF, 4096);
  setvbuf(stdout, nullptr, _IOLBF, 4096);
  setvbuf(stderr, nullptr, _IOLBF, 4096);
}

void Term::Private::FileInitializer::detachConsole()
{
#if defined(_WIN32)
  if(hadToAttachConsole) FreeConsole();
#endif
}

bool Term::Private::FileInitializer::hadToAttachConsole = {false};

void Term::Private::FileInitializer::init()
{
  // MacOS was not happy wish a static mutex in the class so we create it and pass to each class;
  static std::recursive_mutex io_mutex;
  if(m_counter++ == 0)
  {
    attachConsole();
#if defined(_WIN32)
    new(&Term::Private::in) InputFileHandler(io_mutex, "CONIN$");
    new(&Term::Private::out) OutputFileHandler(io_mutex, "CONOUT$");
#else
    new(&Term::Private::in) InputFileHandler(io_mutex, "/dev/tty");
    new(&Term::Private::out) OutputFileHandler(io_mutex, "/dev/tty");
#endif
  }
}

Term::Private::FileInitializer::FileInitializer() { init(); }

Term::Private::FileInitializer::~FileInitializer()
{
  if(--m_counter == 0)
  {
    (&Term::Private::in)->~InputFileHandler();
    (&Term::Private::out)->~OutputFileHandler();
    detachConsole();
  }
}

int Term::Private::OutputFileHandler::write(const std::string& str)
{
  if(str.empty()) return 0;
    //std::lock_guard<std::mutex> lock(m_mut);
#if defined(_WIN32)
  DWORD dwCount{0};
  if(WriteConsole(handle(), &str[0], static_cast<DWORD>(str.size()), &dwCount, nullptr) == 0) return -1;
  else
    return static_cast<int>(dwCount);
#else
  return ::write(fd(), &str[0], str.size());
#endif
}

int Term::Private::OutputFileHandler::write(const char& ch)
{
  //std::lock_guard<std::mutex> lock(m_mut);
#if defined(_WIN32)
  DWORD dwCount{0};
  if(WriteConsole(handle(), &ch, 1, &dwCount, nullptr) == 0) return -1;
  else
    return static_cast<int>(dwCount);
#else
  return ::write(fd(), &ch, 1);
#endif
}

std::string Term::Private::InputFileHandler::read()
{
  //std::lock_guard<std::mutex> lock(m_mut);
#if defined(_WIN32)
  DWORD       nread{0};
  std::string ret(4096, '\0');
  errno = 0;
  ReadConsole(Private::in.handle(), &ret[0], static_cast<DWORD>(ret.size()), &nread, nullptr);
  return ret.c_str();
#else
  std::size_t nread{0};
  ::ioctl(Private::in.fd(), FIONREAD, &nread);
  if(nread != 0)
  {
    std::string ret(nread, '\0');
    errno = 0;
    ::ssize_t nnread{::read(Private::in.fd(), &ret[0], ret.size())};
    if(nnread == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
    return ret.c_str();
  }
  else
    return std::string();
#endif
}

void Term::Private::FileHandler::flush() { std::fflush(m_file); }

void Term::Private::FileHandler::lockIO() { m_mutex.lock(); }
void Term::Private::FileHandler::unlockIO() { m_mutex.unlock(); }
