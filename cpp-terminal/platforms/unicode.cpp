/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/unicode.hpp"

#if defined(_WIN32)
  #include <windows.h>
#endif

#include "cpp-terminal/exception.hpp"

#if defined(_WIN32)
std::wstring Term::Private::utf8_to_wide(const std::string& utf8)
{
  if(utf8.empty()) return std::wstring();
  static constexpr DWORD kFlags{MB_ERR_INVALID_CHARS};
  if(utf8.length() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
  const int utf16Length{::MultiByteToWideChar(CP_UTF8, kFlags, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0)};
  if(utf16Length == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-8 sequence found in input string." : "Cannot get result string length when converting from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }
  std::wstring utf16;
  utf16.reserve(utf16Length);
  int result{::MultiByteToWideChar(CP_UTF8, kFlags, utf8.data(), static_cast<int>(utf8.size()), &utf16[0], utf16Length)};
  if(result == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-8 sequence found in input string." : "Cannot convert from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }
  return utf16;
}
#endif

#if defined(_WIN32)
std::string Term::Private::wide_to_utf8(const std::wstring& utf16)
{
  if(utf16.empty()) return std::string{};
  static constexpr DWORD kFlags{WC_ERR_INVALID_CHARS};
  if(utf16.length() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
  const int utf8Length{::WideCharToMultiByte(CP_UTF8, kFlags, utf16.data(), static_cast<int>(utf16.length()), nullptr, 0, nullptr, nullptr)};
  if(utf8Length == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-16 sequence found in input string." : "Cannot get result string length when converting from UTF-16 to UTF-8 (WideCharToMultiByte failed).");
  }
  std::string utf8;
  utf8.resize(utf8Length);
  int result{::WideCharToMultiByte(CP_UTF8, kFlags, utf16.data(), static_cast<int>(utf16.length()), &utf8[0], utf8Length, nullptr, nullptr)};
  if(result == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-16 sequence found in input string." : "Cannot convert from UTF-16 to UTF-8 (WideCharToMultiByte failed).");
  }
  return utf8;
}
#endif

bool Term::Private::is_valid_utf8_code_unit(const std::string& s)
{
  static const constexpr int b1OOOOOOO{128};
  static const constexpr int b11OOOOOO{192};
  static const constexpr int b111OOOOO{224};
  static const constexpr int b1111OOOO{240};
  static const constexpr int b11111OOO{248};
  switch(s.size())
  {
    case 1: return ((s[0] & b1OOOOOOO) == 0) ? true : false;
    case 2: return ((s[0] & b111OOOOO) == b11OOOOOO) && ((s[1] & b11OOOOOO) == b1OOOOOOO) ? true : false;
    case 3: return ((s[0] & b1111OOOO) == b111OOOOO) && ((s[1] & b11OOOOOO) == b1OOOOOOO) && ((s[2] & b11OOOOOO) == b1OOOOOOO) ? true : false;
    case 4: return ((s[0] & b11111OOO) == b1111OOOO) && ((s[1] & b11OOOOOO) == b1OOOOOOO) && ((s[2] & b11OOOOOO) == b1OOOOOOO) && ((s[3] & b11OOOOOO) == b1OOOOOOO) ? true : false;
    default: return false;
  }
}

static constexpr std::uint8_t UTF8_ACCEPT{0};
static constexpr std::uint8_t UTF8_REJECT{0xf};

std::uint8_t utf8_decode_step(std::uint8_t state, std::uint8_t octet, std::uint32_t* cpp)
{
  static const std::uint32_t utf8ClassTab[0x10] = {
    0x88888888UL, 0x88888888UL, 0x99999999UL, 0x99999999UL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0x222222ffUL, 0x22222222UL, 0x22222222UL, 0x22222222UL, 0x3333333bUL, 0x33433333UL, 0xfff5666cUL, 0xffffffffUL,
  };

  static const std::uint32_t utf8StateTab[0x10] = {
    0xfffffff0UL, 0xffffffffUL, 0xfffffff1UL, 0xfffffff3UL, 0xfffffff4UL, 0xfffffff7UL, 0xfffffff6UL, 0xffffffffUL, 0x33f11f0fUL, 0xf3311f0fUL, 0xf33f110fUL, 0xfffffff2UL, 0xfffffff5UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL,
  };

  const std::uint8_t reject{static_cast<std::uint8_t>(state >> 3)};
  const std::uint8_t nonAscii{static_cast<std::uint8_t>(octet >> 7)};
  const std::uint8_t class_{static_cast<std::uint8_t>(!nonAscii ? 0 : (0xf & (utf8ClassTab[(octet >> 3) & 0xf] >> (4 * (octet & 7)))))};

  *cpp = (state == UTF8_ACCEPT ? (octet & (0xffU >> class_)) : ((octet & 0x3fU) | (*cpp << 6)));

  return (reject ? 0xf : (0xf & (utf8StateTab[class_] >> (4 * (state & 7)))));
}

std::u32string Term::Private::utf8_to_utf32(const std::string& s)
{
  std::uint32_t  codepoint{0};
  std::uint8_t   state{UTF8_ACCEPT};
  std::u32string r;
  for(char i: s)
  {
    state = utf8_decode_step(state, static_cast<std::uint8_t>(i), &codepoint);
    if(state == UTF8_ACCEPT) { r.push_back(codepoint); }
    else if(state == UTF8_REJECT) { throw Term::Exception("Invalid byte in UTF8 encoded string"); }
  }
  if(state != UTF8_ACCEPT) { throw Term::Exception("Expected more bytes in UTF8 encoded string"); }
  return r;
}

std::string Term::Private::utf32_to_utf8(const char32_t& c)
{
  if(c > 0x0010FFFF) { throw Term::Exception("Invalid UTF32 codepoint."); }
  char        bytes[4];
  std::string ret;
  std::size_t nbytes{1};
  char32_t    d{c};
  if(c >= 0x10000)
  {
    nbytes++;
    bytes[3] = static_cast<char>((d | 0x80) & 0xBF);
    d >>= 6;
  }
  if(c >= 0x800)
  {
    nbytes++;
    bytes[2] = static_cast<char>((d | 0x80) & 0xBF);
    d >>= 6;
  }
  if(c >= 0x80)
  {
    nbytes++;
    bytes[1] = static_cast<char>((d | 0x80) & 0xBF);
    d >>= 6;
  }
  static const constexpr unsigned char mask[4] = {0x00, 0xC0, 0xE0, 0xF0};
  bytes[0]                                     = static_cast<char>(d | mask[nbytes - 1]);
  ret.append(bytes, nbytes);
  return ret;
}
