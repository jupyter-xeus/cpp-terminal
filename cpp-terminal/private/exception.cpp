/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/exception.hpp"

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/version.hpp"
#include "return_code.hpp"

#include <atomic>
#include <exception>

#if defined(_WIN32)
  #include "cpp-terminal/private/unicode.hpp"

  #include <memory>
  #include <windows.h>
  #if defined(MessageBox)
    #undef MessageBox
  #endif
#else
  #include <cstring>
#endif

#include <cerrno>
#include <cstdio>
#include <string>

Term::Exception::Exception(const std::string& message) noexcept : m_message(message) {}

Term::Exception::Exception(const std::int64_t& code, const std::string& message) noexcept : m_code(code), m_message(message) {}

const char* Term::Exception::what() const noexcept
{
  build_what();
  return m_what.c_str();
}

std::int64_t Term::Exception::code() const noexcept { return m_code; }

std::string Term::Exception::message() const noexcept { return m_message; }

std::string Term::Exception::context() const noexcept { return m_context; }

Term::Exception::Exception(const std::int64_t& code) noexcept : m_code(code) {}

void Term::Exception::build_what() const noexcept
{
  if(0 == m_code) { m_what = m_message; }
  else { m_what = "error " + std::to_string(m_code) + ": " + m_message; }
}

void Term::Exception::setMessage(const std::string& message) noexcept { m_message = message; }

void Term::Exception::setContext(const std::string& context) noexcept { m_context = context; }

void Term::Exception::setWhat(const std::string& what) const noexcept { m_what = what; }

#if defined(_WIN32)

std::int64_t Term::Private::WindowsError::error() const noexcept { return m_error; }

bool Term::Private::WindowsError::check_value() const noexcept { return m_check_value; }

Term::Private::WindowsError& Term::Private::WindowsError::check_if(const bool& ret) noexcept
{
  m_error       = static_cast<std::int64_t>(GetLastError());
  m_check_value = ret;
  return *this;
}

void Term::Private::WindowsError::throw_exception(const std::string& str) const
{
  if(m_check_value) { throw Term::Private::WindowsException(m_error, str); }
}

Term::Private::WindowsException::WindowsException(const std::int64_t& error, const std::string& context) : Term::Exception(static_cast<std::int64_t>(error))
{
  setContext(context);
  wchar_t*    ptr{nullptr};
  const DWORD cchMsg{FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, static_cast<uint32_t>(code()), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<wchar_t*>(&ptr), 0, nullptr)};
  if(cchMsg > 0)
  {
    auto deleter = [](void* p)
    {
      if(p != nullptr) { ::LocalFree(p); }
    };
    std::unique_ptr<wchar_t, decltype(deleter)> ptrBuffer(ptr, deleter);
    std::string                                 ret{Term::Private::to_narrow(ptrBuffer.get())};
    if(ret.size() >= 2 && ret[ret.size() - 1] == '\n' && ret[ret.size() - 2] == '\r') ret.erase(ret.size() - 2);
    setMessage(ret);
  }
  else { throw Term::Exception(::GetLastError(), "Error in FormatMessageW"); }
}

void Term::Private::WindowsException::build_what() const noexcept
{
  std::string what{std::string("windows error ") + std::to_string(code()) + std::string(": ") + message().c_str()};
  if(!context().empty()) what += " [" + context() + "]";
  setWhat(what);
}

#endif

Term::Private::Errno::Errno() noexcept
{
#if defined(_WIN32)
  _set_errno(0);
#else
  errno = {0};  //NOSONAR
#endif
}

Term::Private::Errno::~Errno() noexcept
{
#if defined(_WIN32)
  _set_errno(0);
#else
  errno = {0};  //NOSONAR
#endif
}

std::int64_t Term::Private::Errno::error() const noexcept { return m_errno; }

bool Term::Private::Errno::check_value() const noexcept { return m_check_value; }

Term::Private::Errno& Term::Private::Errno::check_if(const bool& ret) noexcept
{
#if defined(_WIN32)
  int err{static_cast<int>(m_errno)};
  _get_errno(&err);
#else
  m_errno = static_cast<std::uint32_t>(errno);  //NOSONAR
#endif
  m_check_value = {ret};
  return *this;
}

void Term::Private::Errno::throw_exception(const std::string& str) const
{
  if(m_check_value) { throw Term::Private::ErrnoException(m_errno, str); }
}

Term::Private::ErrnoException::ErrnoException(const std::int64_t& error, const std::string& context) : Term::Exception(error)
{
  setContext(context);
#if defined(_WIN32)
  std::wstring message(m_maxSize, L'\0');
  message = _wcserror_s(&message[0], message.size(), static_cast<int>(error));
  setMessage(Term::Private::to_narrow(message.c_str()));
#else
  std::string message(m_maxSize, '\0');
  message = ::strerror_r(static_cast<std::int32_t>(error), &message[0], message.size());  // NOLINT(readability-container-data-pointer)
  setMessage(message);
#endif
}

void Term::Private::ErrnoException::build_what() const noexcept
{
  std::string what{"errno " + std::to_string(code()) + ": " + message()};
  if(!context().empty()) { what += +" [" + context() + "]"; }
  setWhat(what);
}

void Term::Private::ExceptionHandler(const ExceptionDestination& destination) noexcept
{
  try
  {
    std::exception_ptr exception{std::current_exception()};
    if(exception != nullptr) { std::rethrow_exception(exception); }
  }
  catch(const Term::Exception& exception)
  {
    switch(destination)
    {
      case ExceptionDestination::MessageBox:
#if defined(_WIN32)
        MessageBoxW(nullptr, Term::Private::to_wide(exception.what()).c_str(), Term::Private::to_wide("cpp-terminal v" + Term::Version::string()).c_str(), MB_OK | MB_ICONERROR);
        break;
#endif
      case ExceptionDestination::StdErr:
#if defined(_WIN32)
        (void)(fputws(Term::Private::to_wide(std::string("cpp-terminal v" + Term::Version::string() + "\n" + exception.what() + "\n")).c_str(), stderr));
#else
        (void)(fputs(std::string("cpp-terminal v" + Term::Version::string() + "\n" + exception.what() + "\n").c_str(), stderr));
#endif
        break;
      default: break;
    }
  }
  catch(const std::exception& exception)
  {
    switch(destination)
    {
      case ExceptionDestination::MessageBox:
#if defined(_WIN32)
        MessageBoxW(nullptr, Term::Private::to_wide(exception.what()).c_str(), Term::Private::to_wide("cpp-terminal v" + Term::Version::string()).c_str(), MB_OK | MB_ICONERROR);
        break;
#endif
      case ExceptionDestination::StdErr:
#if defined(_WIN32)
        (void)(fputws(Term::Private::to_wide(std::string("cpp-terminal v" + Term::Version::string() + "\n" + exception.what() + "\n")).c_str(), stderr));
#else
        (void)(fputs(std::string("cpp-terminal v" + Term::Version::string() + "\n" + exception.what() + "\n").c_str(), stderr));
#endif
        break;
      default: break;
    }
  }
  catch(...)
  {
    switch(destination)
    {
      case ExceptionDestination::MessageBox:
#if defined(_WIN32)
        MessageBoxW(nullptr, Term::Private::to_wide("cpp-terminal v" + Term::Version::string() + "Unknown error").c_str(), Term::Private::to_wide("cpp-terminal v" + Term::Version::string()).c_str(), MB_OK | MB_ICONERROR);
        break;
#endif
      case ExceptionDestination::StdErr:
#if defined(_WIN32)
        (void)(fputws(Term::Private::to_wide("cpp-terminal v" + Term::Version::string() + ": Unknown error\n").c_str(), stderr));
#else
        (void)(fputs(("cpp-terminal v" + Term::Version::string() + ": Unknown error\n").c_str(), stderr));
#endif
      default: break;
    }
  }
  (void)(std::fflush(stderr));
  std::_Exit(Term::returnCode());
}
