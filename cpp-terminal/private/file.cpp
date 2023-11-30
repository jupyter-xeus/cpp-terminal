/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/file.hpp"

#include "cpp-terminal/private/exception.hpp"

#include <cstdio>
#include <new>

#if defined(_WIN32)
  #include <io.h>
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/private/unicode.hpp"

#include <array>
#include <fcntl.h>

//FIXME Move this to other file

namespace
{
std::array<char, sizeof(Term::Private::InputFileHandler)>  stdin_buffer;   //NOLINT(fuchsia-statically-constructed-objects)
std::array<char, sizeof(Term::Private::OutputFileHandler)> stdout_buffer;  //NOLINT(fuchsia-statically-constructed-objects)
}  // namespace

Term::Private::InputFileHandler&  Term::Private::in  = reinterpret_cast<Term::Private::InputFileHandler&>(stdin_buffer);
Term::Private::OutputFileHandler& Term::Private::out = reinterpret_cast<Term::Private::OutputFileHandler&>(stdout_buffer);

//

Term::Private::FileHandler::FileHandler(std::recursive_mutex& mutex, const std::string& filename, const std::string& mode) noexcept
try : m_mutex(mutex)
{
#if defined(_WIN32)
  m_handle = {CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
  if(m_handle == INVALID_HANDLE_VALUE)
  {
    Term::Private::WindowsError().check_if((m_handle = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)) == INVALID_HANDLE_VALUE).throw_exception("Problem opening NUL");
    m_null = true;
  }
  Term::Private::Errno().check_if((m_fd = _open_osfhandle(reinterpret_cast<intptr_t>(m_handle), _O_RDWR)) == -1).throw_exception("_open_osfhandle(reinterpret_cast<intptr_t>(m_handle), _O_RDWR)");
  Term::Private::Errno().check_if(nullptr == (m_file = _fdopen(m_fd, mode.c_str()))).throw_exception("_fdopen(m_fd, mode.c_str())");
#else
  std::size_t flag{O_ASYNC | O_DSYNC | O_NOCTTY | O_SYNC | O_NDELAY};
  if(mode.find('r') != std::string::npos) { flag |= O_RDONLY; }       //NOLINT(abseil-string-find-str-contains)
  else if(mode.find('w') != std::string::npos) { flag |= O_WRONLY; }  //NOLINT(abseil-string-find-str-contains)
  else { flag |= O_RDWR; }
  m_fd = {::open(filename.c_str(), static_cast<int>(flag))};  //NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
  if(m_fd == -1)
  {
    Term::Private::Errno().check_if((m_fd = ::open("/dev/null", static_cast<int>(flag))) == -1).throw_exception(R"(::open("/dev/null", flag))");  //NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    m_null = true;
  }
  Term::Private::Errno().check_if(nullptr == (m_file = ::fdopen(m_fd, mode.c_str()))).throw_exception("::fdopen(m_fd, mode.c_str())");
  m_handle = m_file;
#endif
  Term::Private::Errno().check_if(std::setvbuf(m_file, nullptr, _IONBF, 0) != 0).throw_exception("std::setvbuf(m_file, nullptr, _IONBF, 0)");
}
catch(...)
{
  ExceptionHandler(ExceptionDestination::StdErr);
}

Term::Private::FileHandler::~FileHandler() noexcept
try
{
  Term::Private::Errno().check_if(0 != std::fflush(m_file)).throw_exception("std::fflush(m_file)");
  Term::Private::Errno().check_if(0 != std::fclose(m_file)).throw_exception("std::fclose(m_file)");  //NOLINT(cppcoreguidelines-owning-memory)
}
catch(...)
{
  ExceptionHandler(ExceptionDestination::StdErr);
}

bool Term::Private::FileHandler::null() const { return m_null; }

FILE* Term::Private::FileHandler::file() { return m_file; }

std::int32_t Term::Private::FileHandler::fd() const { return m_fd; }

Term::Private::FileHandler::Handle Term::Private::FileHandler::handle() { return m_handle; }

std::size_t Term::Private::OutputFileHandler::write(const std::string& str)
{
  if(str.empty()) { return 0; }
  //std::lock_guard<std::mutex> lock(m_mut);
#if defined(_WIN32)
  DWORD dwCount{0};
  Term::Private::WindowsError().check_if(0 == WriteConsole(handle(), &str[0], static_cast<DWORD>(str.size()), &dwCount, nullptr)).throw_exception("WriteConsole(handle(), &str[0], static_cast<DWORD>(str.size()), &dwCount, nullptr)");
  return static_cast<std::size_t>(dwCount);
#else
  return ::write(fd(), str.data(), str.size());
#endif
}

std::size_t Term::Private::OutputFileHandler::write(const char& ch)
{
  //std::lock_guard<std::mutex> lock(m_mut);
#if defined(_WIN32)
  DWORD dwCount{0};
  Term::Private::WindowsError().check_if(0 == WriteConsole(handle(), &ch, 1, &dwCount, nullptr)).throw_exception("WriteConsole(handle(), &ch, 1, &dwCount, nullptr)");
  return static_cast<std::size_t>(dwCount);
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
  return {};
#endif
}

void Term::Private::FileHandler::flush() { std::fflush(m_file); }

void Term::Private::FileHandler::lockIO() { m_mutex.lock(); }
void Term::Private::FileHandler::unlockIO() { m_mutex.unlock(); }

Term::Private::OutputFileHandler::OutputFileHandler(std::recursive_mutex& io_mutex) noexcept
try : FileHandler(io_mutex, m_file, "w")
{
  //noop
}
catch(...)
{
  ExceptionHandler(ExceptionDestination::StdErr);
}

Term::Private::InputFileHandler::InputFileHandler(std::recursive_mutex& io_mutex) noexcept
try : FileHandler(io_mutex, m_file, "r")
{
  //noop
}
catch(...)
{
  ExceptionHandler(ExceptionDestination::StdErr);
}
