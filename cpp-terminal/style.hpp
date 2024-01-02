/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/iostream.hpp"

#include <cstdint>
#include <string>

namespace Term
{

/*
 * Styles for text in the terminal
 */
enum class Style : std::uint8_t
{

  Reset      = 0,  ///< resets all attributes (styles and colors)
  Bold       = 1,  ///< Thick text font
  Dim        = 2,  ///< lighter, slimmer text font
  Italic     = 3,  ///< slightly bend text font
  Underline  = 4,  ///< draws a line below the text
  Blink      = 5,  ///< Sets blinking to less than 150 times per minute
  BlinkRapid = 6,  ///< MS-DOS ANSI.SYS, 150+ per minute; not widely supported
  Reversed   = 7,  ///< swap foreground and background colors
  Conceal    = 8,  ///< Note: not widely supported
  Crossed    = 9,  ///< strikes through the text, mostly supported

  // different fonts
  Font0     = 10,  ///< Primary or default font
  ResetFont = 10,
  Font1     = 11,
  Font2     = 12,
  Font3     = 13,
  Font4     = 14,
  Font5     = 15,
  Font6     = 16,
  Font7     = 17,
  Font8     = 18,
  Font9     = 19,
  Font10    = 20,  ///< Fraktur / Gothic font

  // Double-underline per ECMA-48,[5] 8.3.117 but instead disables bold intensity on several terminals,
  // including in the Linux kernel's console before version 4.17
  DoublyUnderlinedOrNotBold = 21,

  // resets corresponding styles
  ResetBold       = 22,
  ResetDim        = 22,
  ResetItalic     = 23,
  ResetUnderline  = 24,
  ResetBlink      = 25,
  ResetBlinkRapid = 25,
  ResetReversed   = 27,
  ResetConceal    = 28,
  ResetCrossed    = 29,

  // sets the foreground and background color to the implementation defined colors
  DefaultForegroundColor = 39,
  DefaultBackgroundColor = 49,

  Frame         = 51,
  Encircle      = 52,
  Overline      = 53,  // draw a line over the text, barely supported
  ResetFrame    = 54,
  ResetEncircle = 54,
  ResetOverline = 55,

  // sets the underline color to the implementation defined colors
  DefaultUnderlineColor = 59,  ///< non standard, implemented in Kitty, VTE, mintty, and iTerm2

  BarRight       = 60,  ///< draw a vertical bar on the right side of the character
  DoubleBarRight = 61,  ///< draw a double vertical bar to the right
  BarLeft        = 62,  ///< draw a vertical bar on the left side of the character
  DoubleBarLeft  = 63,  ///< draw a double vertical bar to the left
  StressMarking  = 64,  ///< reset all bars left and right double and simple

  ResetBar = 65,  // resets 60 - 64 inclusive

  Superscript      = 73,  // only implemented in mintty
  Subscript        = 74,  // only implemented in mintty
  ResetSuperscript = 75,  // only implemented in mintty
  ResetSubscript   = 75
};

std::string style(const Term::Style& style);

template<class Stream> Stream& operator<<(Stream& stream, const Term::Style& style_type) { return stream << style(style_type); }
// unabigify operator overload
inline Term::TOstream&         operator<<(Term::TOstream& term, const Term::Style& style_type) { return term << style(style_type); }

}  // namespace Term
