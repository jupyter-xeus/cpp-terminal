/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
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

  RESET       = 0,  // resets all attributes (styles and colors)
  BOLD        = 1,  // Thick text font
  DIM         = 2,  // lighter, slimmer text font
  ITALIC      = 3,  // slightly bend text font
  UNDERLINE   = 4,  // draws a line below the text
  BLINK       = 5,  // Sets blinking to less than 150 times per minute
  BLINK_RAPID = 6,  // MS-DOS ANSI.SYS, 150+ per minute; not widely supported
  REVERSED    = 7,  // swap foreground and background colors
  CONCEAL     = 8,  // Note: not widely supported
  CROSSED     = 9,  // strikes through the text, mostly supported

  // different fonts
  FONT_0  = 10,  // Primary or default font
  FONT_1  = 11,
  FONT_2  = 12,
  FONT_3  = 13,
  FONT_4  = 14,
  FONT_5  = 15,
  FONT_6  = 16,
  FONT_7  = 17,
  FONT_8  = 18,
  FONT_9  = 19,
  FONT_10 = 20,  // Fraktur / Gothic font

  // Double-underline per ECMA-48,[5] 8.3.117 but instead disables bold intensity on several terminals,
  // including in the Linux kernel's console before version 4.17
  DOUBLY_UNDERLINED_OR_NOT_BOLD = 21,

  // resets corresponding styles
  RESET_BOLD_DIM  = 22,
  RESET_ITALIC    = 23,
  RESET_UNDERLINE = 24,
  RESET_BLINKING  = 25,
  RESET_REVERSED  = 27,
  RESET_CONCEAL   = 28,
  RESET_CROSSED   = 29,

  // sets the foreground and background color to the implementation defined colors
  DEFAULT_FOREGROUND_COLOR = 39,
  DEFAULT_BACKGROUND_COLOR = 49,

  FRAME                = 51,
  ENCIRCLE             = 52,
  OVERLINE             = 53,  // draws a line over the text, barely supported
  RESET_FRAME_ENCIRCLE = 54,
  RESET_OVERLINE       = 55,

  // sets the underline color to the implementation defined colors
  DEFAULT_UNDERLINE_COLOR = 59,  // non standard, implemented in Kitty, VTE, mintty, and iTerm2

  BAR_RIGHT        = 60,  // draws a vertical bar on the right side of the character
  DOUBLE_BAR_RIGHT = 61,  // drawa a double vertical bar to the right
  BAR_LEFT         = 62,  // draws a vertical bar on the left side of the character
  DOUBLE_BAR_LEFT  = 63,  // drawa a double vertical bar to the left
  STRESS_MARKING   = 64,  // resets all bars left and right double and simple

  RESET_BAR = 65,  // resets 60 - 64 inclusive

  SUPERSCRIPT                 = 73,  // only implemented in mintty
  SUBSCRIPT                   = 74,  // only implemented in mintty
  RESET_SUPERSCRIPT_SUBSCRIPT = 75   // only implemented in mintty

};

std::string style(const Term::Style& style);

template<class Stream> Stream& operator<<(Stream& stream, const Term::Style& s) { return stream << style(s); }
// unabigify operator overload
inline Term::TOstream&         operator<<(Term::TOstream& term, const Term::Style& s) { return term << style(s); }

}  // namespace Term
