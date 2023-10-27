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

#if defined(_WIN32)
Term::Private::WindowsError::WindowsError(const unsigned long& error, const std::string& context) : Term::Exception(static_cast<std::int64_t>(error))
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

void Term::Private::WindowsError::build_what()
{
  m_what = "windows error " + std::to_string(m_code) + ": " + m_message;
  if(!m_context.empty()) m_what += +" [" + m_context + "]";
}

#endif

Term::Private::Errno::Errno()
{
#if defined(_WIN32)
  _set_doserrno(0);
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
  m_errno = _get_errno();
#else
  m_errno = errno;
#endif
  m_check_value = ret;
  return *this;
}

bool Term::Private::Errno::check_value() const { return m_check_value; }

std::int32_t Term::Private::Errno::error() const { return m_errno; }

Term::Private::ErrnoException::~ErrnoException() = default;

Term::Private::ErrnoException::ErrnoException(const std::int32_t& error, const std::string& context) : Term::Exception(static_cast<std::int64_t>(error))
{
  m_context = context;
#if defined(_WIN32)
  _wcserror_s 
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
