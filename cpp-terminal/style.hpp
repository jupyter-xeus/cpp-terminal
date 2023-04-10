#pragma once

#include <cstdint>
#include <string>

namespace Term
{

/*
 * Styles for text in the terminal
 */
enum class Style : std::uint8_t
{
  // resets all attributes (styles and colors)
  RESET       = 0,
  // Thick text font
  BOLD        = 1,
  // lighter, slimmer text font
  DIM         = 2,
  // slightly bend text font
  ITALIC      = 3,
  // draws a line below the text
  UNDERLINE   = 4,
  BLINK       = 5,
  BLINK_RAPID = 6,
  REVERSED    = 7,
  CONCEAL     = 8,
  // strikes through the text, mostly supported
  CROSSED     = 9,
  // draws a line over the text, barely supported
  OVERLINE    = 53
};

std::string style(const Term::Style& style);

}  // namespace Term
