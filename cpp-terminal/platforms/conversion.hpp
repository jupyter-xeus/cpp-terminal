#pragma once

#include <cstdint>
#include <string>
#include <vector>

#if __cplusplus >= 201703L
  #define TERM_CONSTEXPR constexpr
#else
  #define TERM_CONSTEXPR
#endif

namespace Term
{
namespace Private
{

#if defined(_WIN32)
std::string to_utf8(const wchar_t* utf16Str);
#endif

std::uint8_t utf8_decode_step(std::uint8_t state, std::uint8_t octet, std::uint32_t* cpp);

void codepoint_to_utf8(std::string& s, char32_t c);

std::u32string utf8_to_utf32(const std::string& s);

std::string utf32_to_utf8(const std::u32string& s);

// converts a vector of char into a string
std::string vector_to_string(const std::vector<char>& vector);

enum class Identifier : std::int8_t
{
  Unsupported = -1,
  NotFirst    = 0,
  Bytes1      = 1,
  Bytes2      = 2,
  Bytes3      = 3,
  Bytes4      = 4
};

/// returns the length of the utf8 sequence if it is the first character
/// returns '0' if it is a character in the sequence but not the first one
/// returns '-1' if it is an unsupported character by the utf8 standard
TERM_CONSTEXPR inline Identifier identify(char c)
{
  const auto       prefix = (c & 0xF8) >> 3;
  const Identifier lookupTable[32]{
    /*00000*/ Identifier::Bytes1,
    /*00001*/ Identifier::Bytes1,
    /*00010*/ Identifier::Bytes1,
    /*00011*/ Identifier::Bytes1,
    /*00100*/ Identifier::Bytes1,
    /*00101*/ Identifier::Bytes1,
    /*00110*/ Identifier::Bytes1,
    /*00111*/ Identifier::Bytes1,
    /*01000*/ Identifier::Bytes1,
    /*01001*/ Identifier::Bytes1,
    /*01010*/ Identifier::Bytes1,
    /*01011*/ Identifier::Bytes1,
    /*01100*/ Identifier::Bytes1,
    /*01101*/ Identifier::Bytes1,
    /*01110*/ Identifier::Bytes1,
    /*01111*/ Identifier::Bytes1,

    /*10000*/ Identifier::NotFirst,
    /*10001*/ Identifier::NotFirst,
    /*10010*/ Identifier::NotFirst,
    /*10011*/ Identifier::NotFirst,
    /*10100*/ Identifier::NotFirst,
    /*10101*/ Identifier::NotFirst,
    /*10110*/ Identifier::NotFirst,
    /*10111*/ Identifier::NotFirst,
    /*11000*/ Identifier::Bytes2,
    /*11001*/ Identifier::Bytes2,
    /*11010*/ Identifier::Bytes2,
    /*11011*/ Identifier::Bytes2,
    /*11100*/ Identifier::Bytes3,
    /*11101*/ Identifier::Bytes3,
    /*11110*/ Identifier::Bytes4,
    /*11111*/ Identifier::Unsupported,
  };
  return lookupTable[prefix];
}

}  // namespace Private

// given a range [first, last) this function will parse the first utf8 character and writes it 'out'.
// if 'first' points to a correct utf8 symbol then the function will return a pointer to then next symbol
// if 'first' does not point to a correct utf8 symbol or the symbol would extend over 'last' then the function returns 'first'
template<class CharItr> TERM_CONSTEXPR CharItr utf8_to_utf32(const CharItr first, const CharItr last, std::int32_t* out)
{
  const Private::Identifier id = Private::identify(*first);
  switch(id)
  {
    case Private::Identifier::Bytes1:
      if(std::distance(first, last) >= 1)
      {
        *out = static_cast<std::int32_t>(first[0]);
        return first + 1;
      }

      break;
    case Private::Identifier::Bytes2:
      if(std::distance(first, last) >= 2)
      {
        if((first[1] & 0xC0) == 0xC0)
        {
          *out = (static_cast<std::int32_t>(first[0] & 0x1F) << 6) | static_cast<std::int32_t>(first[1] & 0x3F);
          return first + 2;
        }
      }

      break;
    case Private::Identifier::Bytes3:
      if(std::distance(first, last) >= 3)
      {
        if((first[1] & 0xC0) == 0xC0 && (first[2] & 0xC0) == 0xC0)
        {
          *out = (static_cast<std::int32_t>(first[0] & 0x0F) << 12) | (static_cast<std::int32_t>(first[1] & 0x3F) << 6) | (static_cast<std::int32_t>(first[2] & 0x3F));
          return first + 3;
        }
      }

      break;
    case Private::Identifier::Bytes4:
      if(std::distance(first, last) >= 4)
      {
        if((first[1] & 0xC0) == 0xC0 && (first[2] & 0xC0) == 0xC0 && (first[3] & 0xC0) == 0xC0)
        {
          *out = (static_cast<std::int32_t>(first[0] & 0x07) << 24) | (static_cast<std::int32_t>(first[1] & 0x3F) << 12) | (static_cast<std::int32_t>(first[2] & 0x3F) << 6) | (static_cast<std::int32_t>(first[3] & 0x3F));
          return first + 4;
        }
      }

      break;
    case Private::Identifier::Unsupported: break; break;
    default: break;
  }
  return first;
}

}  // namespace Term

#undef TERM_CONSTEXPR
