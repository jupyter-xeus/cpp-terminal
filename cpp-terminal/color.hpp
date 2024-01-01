/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
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

  ///
  /// @brief The 3bit/4bit colors for the terminal.
  /// Get the foreground color: Color4 + 30, Background color: Color4 + 40
  /// See https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit.
  ///
  enum class Name : std::uint8_t
  {
    Black         = 0,   ///< Black FG: 30, BG: 40
    Red           = 1,   ///< Red FG: 31, BG: 41
    Green         = 2,   ///< Green FG: 32, BG: 42
    Yellow        = 3,   ///< Yellow FG: 33, BG: 43
    Blue          = 4,   ///< Blue FG: 34, BG: 44
    Magenta       = 5,   ///< Magenta FG: 35, BG: 45
    Cyan          = 6,   ///< Cyan FG: 36, BG: 46
    White         = 7,   ///< White FG: 37, BG: 47
    Default       = 9,   ///< Use the default terminal color, FG: 39, BG: 49
    Gray          = 60,  ///< Gray FG: 90, BG: 100
    BrightBlack   = 60,  ///< BrightBlack FG: 90, BG: 100
    BrightRed     = 61,  ///< BrightRed FG: 91, BG: 101
    BrightGreen   = 62,  ///< BrightGreen FG: 92, BG: 102
    BrightYellow  = 63,  ///< BrightYellow FG: 93, BG: 103
    BrightBlue    = 64,  ///< BrightBlue FG: 94, BG: 104
    BrightMagenta = 65,  ///< BrightMagenta FG: 95, BG: 105
    BrightCyan    = 66,  ///< BrightCyan FG: 96, BG: 106
    BrightWhite   = 67   ///< BrightWhite FG: 97, BG: 107
  };
  bool operator==(const Color&) const;
  bool operator!=(const Color&) const;
  Color();
  Color(const Term::Color::Name& name);
  Color(const std::uint8_t& color);
  Color(const std::uint8_t& red, const std::uint8_t& green, const std::uint8_t& blue);
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

std::string color_bg(const Term::Color::Name& name);
std::string color_bg(const std::uint8_t& value);
std::string color_bg(const std::uint8_t& red, const std::uint8_t& green, const std::uint8_t& blue);
std::string color_bg(const Color& color);

std::string color_fg(const Term::Color::Name& name);
std::string color_fg(const std::uint8_t& value);
std::string color_fg(const std::uint8_t& red, const std::uint8_t& green, const std::uint8_t& blue);
std::string color_fg(const Color& color);

}  // namespace Term
