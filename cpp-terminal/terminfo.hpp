/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>

namespace Term
{

class Terminfo
{
public:
  // indicates the color mode (basically the original color resolution)
  // also used to manually override the original color resolution
  enum class ColorMode
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
  Terminfo();
  static ColorMode getColorMode();
  bool             hasANSIEscapeCode() const;
  bool             isLegacy() const;
  bool             hasUTF8();
  void             checkUTF8();
  std::string      getName();

private:
  void             setANSIEscapeCode();
  void             setColorMode();
  void             setLegacy();
  bool             m_ANSIEscapeCode{true};
  bool             m_legacy{false};
  bool             m_UTF8{false};
  static ColorMode m_colorMode;
  std::string      m_terminalName;
  std::string      m_terminalVersion;
  std::string      m_term;
};

}  // namespace Term
