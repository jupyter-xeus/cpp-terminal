/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/conversion.hpp"

#include "cpp-terminal/exception.hpp"

#include <array>
#include <string>

namespace Term
{
namespace Private
{

static constexpr std::uint8_t UTF8_ACCEPT{0};
static constexpr std::uint8_t UTF8_REJECT{0xf};

std::uint8_t utf8_decode_step(std::uint8_t state, std::uint8_t octet, std::uint32_t* cpp)
{
  static const constexpr std::array<std::uint32_t, 0x10> utf8ClassTab{0x88888888UL, 0x88888888UL, 0x99999999UL, 0x99999999UL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0x222222ffUL, 0x22222222UL, 0x22222222UL, 0x22222222UL, 0x3333333bUL, 0x33433333UL, 0xfff5666cUL, 0xffffffffUL};

  static const constexpr std::array<std::uint32_t, 0x10> utf8StateTab{0xfffffff0UL, 0xffffffffUL, 0xfffffff1UL, 0xfffffff3UL, 0xfffffff4UL, 0xfffffff7UL, 0xfffffff6UL, 0xffffffffUL, 0x33f11f0fUL, 0xf3311f0fUL, 0xf33f110fUL, 0xfffffff2UL, 0xfffffff5UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL};

  const std::uint8_t reject{static_cast<std::uint8_t>(state >> 3UL)};
  const std::uint8_t nonAscii{static_cast<std::uint8_t>(octet >> 7UL)};
  const std::uint8_t class_{static_cast<std::uint8_t>(!nonAscii ? 0 : (0xf & (utf8ClassTab[(octet >> 3) & 0xf] >> (4 * (octet & 7)))))};

  *cpp = (state == UTF8_ACCEPT ? (octet & (0xffU >> class_)) : ((octet & 0x3fU) | (*cpp << 6)));

  return (reject ? 0xf : (0xf & (utf8StateTab[class_] >> (4 * (state & 7)))));
}

std::u32string utf8_to_utf32(const std::string& str)
{
  std::uint32_t  codepoint{0};
  std::uint8_t   state{UTF8_ACCEPT};
  std::u32string ret;
  for(char idx: str)
  {
    state = utf8_decode_step(state, static_cast<std::uint8_t>(idx), &codepoint);
    if(state == UTF8_ACCEPT) { ret.push_back(codepoint); }
    else if(state == UTF8_REJECT) { throw Term::Exception("Invalid byte in UTF8 encoded string"); }
  }
  if(state != UTF8_ACCEPT) { throw Term::Exception("Expected more bytes in UTF8 encoded string"); }
  return ret;
}

bool is_valid_utf8_code_unit(const std::string& str)
{
  static const constexpr std::uint8_t b1OOOOOOO{128};
  static const constexpr std::uint8_t b11OOOOOO{192};
  static const constexpr std::uint8_t b111OOOOO{224};
  static const constexpr std::uint8_t b1111OOOO{240};
  static const constexpr std::uint8_t b11111OOO{248};
  switch(str.size())
  {
    case 1: return (static_cast<std::uint8_t>(str[0]) & b1OOOOOOO) == 0;
    case 2: return ((static_cast<std::uint8_t>(str[0]) & b111OOOOO) == b11OOOOOO) && ((static_cast<std::uint8_t>(str[1]) & b11OOOOOO) == b1OOOOOOO);
    case 3: return ((static_cast<std::uint8_t>(str[0]) & b1111OOOO) == b111OOOOO) && ((static_cast<std::uint8_t>(str[1]) & b11OOOOOO) == b1OOOOOOO) && ((static_cast<std::uint8_t>(str[2]) & b11OOOOOO) == b1OOOOOOO);
    case 4: return ((static_cast<std::uint8_t>(str[0]) & b11111OOO) == b1111OOOO) && ((static_cast<std::uint8_t>(str[1]) & b11OOOOOO) == b1OOOOOOO) && ((static_cast<std::uint8_t>(str[2]) & b11OOOOOO) == b1OOOOOOO) && ((static_cast<std::uint8_t>(str[3]) & b11OOOOOO) == b1OOOOOOO);
    default: return false;
  }
}

}  // namespace Private

}  // namespace Term
