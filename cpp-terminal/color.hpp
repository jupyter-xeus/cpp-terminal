/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/terminfo.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace Term
{

class Color
{
public:
  enum class Type : std::uint8_t
  {
    Unset,
    NoColor,
    Bit3,
    Bit4,
    Bit8,
    Bit24
  };
  /*
 * The 3bit/4bit colors for the terminal
 * get the foreground color: Color4 + 30, Background color: Color4 + 40
 * See https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
 */
  enum class Name : std::uint8_t
  {
    // FG: 30, BG: 40
    Black         = 0,
    // FG: 31, BG: 41
    Red           = 1,
    // FG: 32, BG: 42
    Green         = 2,
    // FG: 33, BG: 43
    Yellow        = 3,
    // FG: 34, BG: 44
    Blue          = 4,
    // FG: 35, BG: 45
    Magenta       = 5,
    // FG: 36, BG: 46
    Cyan          = 6,
    // FG: 37, BG: 47
    White         = 7,
    // Use the default terminal color, FG: 39, BG: 49
    Default       = 9,
    // FG: 90, BG: 100
    Gray          = 60,
    BrightBlack   = 60,
    // FG: 91, BG: 101
    BrightRed     = 61,
    // FG: 92, BG: 102
    BrightGreen   = 62,
    // FG: 93, BG: 103
    BrightYellow  = 63,
    // FG: 94, BG: 104
    BrightBlue    = 64,
    // FG: 95, BG: 105
    BrightMagenta = 65,
    // FG: 96, BG: 106
    BrightCyan    = 66,
    // FG: 97, BG: 107
    BrightWhite   = 67
  };
  bool operator==(const Color&) const;
  bool operator!=(const Color&) const;
  Color();
  Color(const Term::Color::Name&);
  Color(const std::uint8_t&);
  Color(const std::uint8_t&, const std::uint8_t&, const std::uint8_t&);
  Type                        getType() const;
  Name                        to3bits() const;
  Name                        to4bits() const;
  std::uint8_t                to8bits() const;
  std::array<std::uint8_t, 3> to24bits() const;

private:
  Type m_Type{Type::Unset};
  union
  {
    std::uint8_t                m_bit8;
    std::array<std::uint8_t, 3> m_bit24;
  };
};

std::string color_bg(const Term::Color::Name&);
std::string color_bg(const std::uint8_t&);
std::string color_bg(const std::uint8_t&, const std::uint8_t&, const std::uint8_t&);
std::string color_bg(const Color&);

std::string color_fg(const Term::Color::Name&);
std::string color_fg(const std::uint8_t&);
std::string color_fg(const std::uint8_t&, const std::uint8_t&, const std::uint8_t&);
std::string color_fg(const Color&);

}  // namespace Term
