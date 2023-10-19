/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/unicode.hpp"

#include "cpp-terminal/platforms/exception.hpp"

#if defined(_WIN32)
  #include <limits>
  #include <windows.h>
#endif

#if defined(_WIN32)
std::string Term::Private::to_narrow(const std::wstring& in)
{
  if(in.empty()) return std::string();
  static constexpr DWORD flag{WC_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0, nullptr, nullptr)};
  if(ret_size == 0) throw Term::Private::WindowsError(::GetLastError());
  std::string ret(static_cast<std::size_t>(ret_size), '\0');
  int         ret_error{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size, nullptr, nullptr)};
  if(ret_error == 0) throw Term::Private::WindowsError(::GetLastError());
  return ret;
}

std::wstring Term::Private::to_wide(const std::string& in)
{
  if(in.empty()) return std::wstring();
  static constexpr DWORD flag{MB_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0)};
  if(ret_size == 0) throw Term::Private::WindowsError(::GetLastError());
  std::wstring ret(static_cast<std::size_t>(ret_size), '\0');
  int          ret_error{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size)};
  if(ret_error == 0) throw Term::Private::WindowsError(::GetLastError());
  return ret;
}
#endif
