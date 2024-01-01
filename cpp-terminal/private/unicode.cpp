/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/unicode.hpp"

#include "cpp-terminal/private/exception.hpp"

#if defined(_WIN32)
  #include <limits>
  #include <windows.h>
#endif

#include <array>

#if defined(_WIN32)
std::string Term::Private::to_narrow(const std::wstring& in)
{
  if(in.empty()) return std::string();
  static constexpr DWORD flag{WC_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0, nullptr, nullptr)};
  if(ret_size == 0) throw Term::Private::WindowsException(::GetLastError());
  std::string ret(static_cast<std::size_t>(ret_size), '\0');
  int         ret_error{::WideCharToMultiByte(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size, nullptr, nullptr)};
  if(ret_error == 0) throw Term::Private::WindowsException(::GetLastError());
  return ret;
}

std::wstring Term::Private::to_wide(const std::string& in)
{
  if(in.empty()) return std::wstring();
  static constexpr DWORD flag{MB_ERR_INVALID_CHARS};
  std::size_t            in_size{in.size()};
  if(in_size > static_cast<size_t>((std::numeric_limits<int>::max)())) throw Term::Exception("String size is to big " + std::to_string(in_size) + "/" + std::to_string((std::numeric_limits<int>::max)()));
  const int ret_size{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), nullptr, 0)};
  if(ret_size == 0) throw Term::Private::WindowsException(::GetLastError());
  std::wstring ret(static_cast<std::size_t>(ret_size), '\0');
  int          ret_error{::MultiByteToWideChar(CP_UTF8, flag, in.data(), static_cast<int>(in_size), &ret[0], ret_size)};
  if(ret_error == 0) throw Term::Private::WindowsException(::GetLastError());
  return ret;
}
#endif

std::string Term::Private::utf32_to_utf8(const char32_t& codepoint, const bool& exception)
{
  static const constexpr std::array<std::uint32_t, 4> size{0x7F, 0x07FF, 0xFFFF, 0x10FFFF};
  static const constexpr std::uint8_t                 mask{0x80};
  static const constexpr std::uint8_t                 add{0x3F};
  static const constexpr std::array<std::uint8_t, 3>  mask_first{0x1F, 0x0F, 0x07};
  static const constexpr std::array<std::uint8_t, 3>  add_first{0xC0, 0xE0, 0xF0};
  static const constexpr std::array<std::uint8_t, 4>  shift{0, 6, 12, 18};
  static const constexpr std::uint8_t                 max_size{4};
  std::string                                         ret;
  ret.reserve(max_size);
  if(codepoint <= size[0]) { ret = {static_cast<char>(codepoint)}; }  // Plain ASCII
  else if(codepoint <= size[1]) { ret = {static_cast<char>(((codepoint >> shift[1]) & mask_first[0]) | add_first[0]), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(codepoint <= size[2]) { ret = {static_cast<char>(((codepoint >> shift[2]) & mask_first[1]) | add_first[1]), static_cast<char>(((codepoint >> shift[1]) & add) | mask), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(codepoint <= size[3]) { ret = {static_cast<char>(((codepoint >> shift[3]) & mask_first[2]) | add_first[2]), static_cast<char>(((codepoint >> shift[2]) & add) | mask), static_cast<char>(((codepoint >> shift[1]) & add) | mask), static_cast<char>(((codepoint >> shift[0]) & add) | mask)}; }
  else if(exception) { throw Term::Exception("Invalid UTF32 codepoint."); }
  else { ret = "\xEF\xBF\xBD"; }
  return ret;
}

std::string Term::Private::utf32_to_utf8(const std::u32string& str, const bool& exception)
{
  std::string ret;
  for(const char32_t codepoint: str) { ret.append(utf32_to_utf8(codepoint, exception)); }
  return ret;
}
