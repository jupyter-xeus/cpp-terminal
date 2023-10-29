/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/exception.hpp"

#include "cpp-terminal/platforms/unicode.hpp"

#if defined(_WIN32)
  #include <memory>
  #include <windows.h>
#else
  #include <cstring>
#endif

#include <iostream>

#if defined(_WIN32)
Term::Private::WindowsError::WindowsError() = default;

Term::Private::WindowsError::~WindowsError() = default;

std::int32_t Term::Private::WindowsError::error() const { return m_error; }

bool Term::Private::WindowsError::check_value() const { return m_check_value; }

Term::Private::WindowsError& Term::Private::WindowsError::check_if(const bool& ret)
{
  m_error       = GetLastError();
  m_check_value = ret;
  return *this;
}

void Term::Private::WindowsError::throw_exception(const std::string& str)
{
  if(m_check_value) { throw Term::Private::WindowsException(m_error, str); }
}

Term::Private::WindowsException::WindowsException(const unsigned long& error, const std::string& context) : Term::Exception(static_cast<std::int64_t>(error))
{
  m_context = context;
  wchar_t*    ptr{nullptr};
  const DWORD cchMsg{FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, static_cast<uint32_t>(m_code), 0, reinterpret_cast<wchar_t*>(&ptr), 0, nullptr)};
  if(cchMsg > 0)
  {
    auto deleter = [](void* p)
    {
      if(p != nullptr) { ::LocalFree(p); }
    };
    std::unique_ptr<wchar_t, decltype(deleter)> ptrBuffer(ptr, deleter);
    std::string                                 ret{Term::Private::to_narrow(ptrBuffer.get())};
    if(ret.size() >= 2 && ret[ret.size() - 1] == '\n' && ret[ret.size() - 2] == '\r') ret.erase(ret.size() - 2);
    m_message = ret;
    build_what();
  }
  else { throw Term::Exception(::GetLastError(), "Error in FormatMessageW"); }
}

void Term::Private::WindowsException::build_what()
{
  m_what = "windows error " + std::to_string(m_code) + ": " + m_message;
  if(!m_context.empty()) m_what += +" [" + m_context + "]";
}

#endif

Term::Private::Errno::Errno()
{
#if defined(_WIN32)
  _set_errno(0);
#else
  errno = 0;
#endif
}

void Term::Private::Errno::throw_exception(const std::string& str)
{
  if(m_check_value) throw Term::Private::ErrnoException(m_errno, str);
}

Term::Private::Errno::~Errno()
{
#if defined(_WIN32)
  _set_errno(0);
#else
  errno = 0;
#endif
}

Term::Private::Errno& Term::Private::Errno::check_if(const bool& ret)
{
#if defined(_WIN32)
  _get_errno(&m_errno);
#else
  m_errno = errno;
#endif
  m_check_value = ret;
  return *this;
}

bool Term::Private::Errno::check_value() const { return m_check_value; }

std::int32_t Term::Private::Errno::error() const { return m_errno; }

Term::Private::ErrnoException::~ErrnoException() = default;

Term::Private::ErrnoException::ErrnoException(const int& error, const std::string& context) : Term::Exception(static_cast<std::int64_t>(error))
{
  m_context = context;
#if defined(_WIN32)
  std::wstring mess;
  mess.assign(256, L'\0');
  _wcserror_s(&mess[0], mess.size(), static_cast<int>(error));
  m_message = Term::Private::to_narrow(mess.c_str());
#else
  m_message.assign(256, '\0');
  m_message = ::strerror_r(error, &m_message[0], m_message.size());
#endif
  build_what();
}

void Term::Private::ErrnoException::build_what()
{
  m_what = "errno " + std::to_string(m_code) + ": " + m_message;
  if(!m_context.empty()) m_what += +" [" + m_context + "]";
}

Term::Exception::Exception(const std::string& message) : m_message(message) { build_what(); }

Term::Exception::Exception(const std::int64_t& code, const std::string& message) : m_code(code), m_message(message) { build_what(); }

Term::Exception::Exception(const std::int64_t& code) : m_code(code) {}

Term::Exception::~Exception() = default;

const char* Term::Exception::what() const noexcept { return m_what.c_str(); }

void Term::Exception::build_what()
{
  if(m_code == 0) { m_what = m_message; }
  else { m_what = "error " + std::to_string(m_code) + ": " + m_message; }
}

std::int64_t Term::Exception::code() const noexcept { return m_code; }

std::string Term::Exception::message() const noexcept { return m_message; }

std::string Term::Exception::context() const noexcept { return m_context; }
