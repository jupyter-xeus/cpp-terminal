/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace Term
{

class Terminfo
{
public:
  // indicates the color mode (basically the original color resolution)
  // also used to manually override the original color resolution
  enum class ColorMode : std::uint8_t
  {
    Unset,
    // no color was used
    NoColor,
    // a 3bit color was used
    Bit3,
    // a 4bit color was used
    Bit4,
    // a 8bit color was used
    Bit8,
    // a 24bit (RGB) color was used
    Bit24
  };
  enum class Bool : std::uint8_t
  {
    UTF8 = 0,          ///< terminal has UTF-8 activated.
    Legacy,            ///< Terminal is in legacy mode (Windows only).
    ControlSequences,  ///< Terminal support control sequences.
  };
  enum class String : std::uint8_t
  {
    TermEnv,      ///< TERM environment variable value.
    TermName,     ///< Name of the terminal program if available.
    TermVersion,  ///< Terminal version.
  };
  enum class Integer : std::uint8_t
  {

  };

  static bool          get(const Term::Terminfo::Bool& key);
  static std::uint32_t get(const Term::Terminfo::Integer& key);
  static std::string   get(const Term::Terminfo::String& key);

private:
  static const constexpr std::size_t BoolNumber{3};
  static const constexpr std::size_t StringNumber{3};
  static const constexpr std::size_t IntegerNumber{0};

public:
  using Booleans = std::array<bool, BoolNumber>;
  using Strings  = std::array<std::string, StringNumber>;
  using Integers = std::array<std::uint32_t, IntegerNumber>;

  Terminfo();

  static ColorMode getColorMode();

private:
  static void check();
  static void checkTermEnv();
  static void checkTerminalName();
  static void checkTerminalVersion();
  static void checkColorMode();
  static void checkUTF8();
  static void checkLegacy();
  static void checkControlSequences();

  static void set(const Term::Terminfo::Bool& key, const bool& value);
  static void set(const Term::Terminfo::Integer& key, const std::uint32_t& value);
  static void set(const Term::Terminfo::String& key, const std::string& value);

  static ColorMode m_colorMode;
  static Booleans  m_booleans;
  static Integers  m_integers;
  static Strings   m_strings;
};

}  // namespace Term
