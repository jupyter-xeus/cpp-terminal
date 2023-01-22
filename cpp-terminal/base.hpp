#pragma once

#include "cpp-terminal/platforms/macros.hpp"
#include "cpp-terminal/platforms/platform.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

namespace Term
{
/*
 * The 3bit/4bit colors for the terminal
 * get the foreground color: Color4 + 30, Background color: Color4 + 40
 * See https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
 */
enum class Color4 : std::uint8_t
{
  // FG: 30, BG: 40
  BLACK          = 0,
  // FG: 31, BG: 41
  RED            = 1,
  // FG: 32, BG: 42
  GREEN          = 2,
  // FG: 33, BG: 43
  YELLOW         = 3,
  // FG: 34, BG: 44
  BLUE           = 4,
  // FG: 35, BG: 45
  MAGENTA        = 5,
  // FG: 36, BG: 46
  CYAN           = 6,
  // FG: 37, BG: 47
  WHITE          = 7,
  // Use the default terminal color, FG: 39, BG: 49
  DEFAULT        = 9,
  // FG: 90, BG: 100
  GRAY           = 60,
  // FG: 91, BG: 101
  RED_BRIGHT     = 61,
  // FG: 92, BG: 102
  GREEN_BRIGHT   = 62,
  // FG: 93, BG: 103
  YELLOW_BRIGHT  = 63,
  // FG: 94, BG: 104
  BLUE_BRIGHT    = 64,
  // FG: 95, BG: 105
  MAGENTA_BRIGHT = 65,
  // FG: 96, BG: 106
  CYAN_BRIGHT    = 66,
  // FG: 97, BG: 107
  WHITE_BRIGHT   = 67
};
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

// Represents a RGB (24bit) color
class rgb
{
public:
  rgb() = default;
  rgb(const std::uint8_t& rr, const std::uint8_t& gg, const std::uint8_t& bb) : r(rr), g(gg), b(bb), empty(false) {}
  std::uint8_t r{0};
  std::uint8_t g{0};
  std::uint8_t b{0};
  bool         empty{true};
};

// indicates the color mode (basically the original color resolution)
// also used to manually override the original color resolution
enum class Mode
{
  // no color was used
  UNSET,
  // a 4bit color was used
  BIT4,
  // a 8bit color was used
  BIT8,
  // a 24bit (RGB) color was used
  BIT24,
  // use 24bit colors, but fall back to 8bit if unsupported
  AUTO24
};
// represents an RGB foreground and background color
struct rgbf
{
  rgbf() = default;
  rgbf(const rgb& rfg, const Mode& mfg, const rgb& rfb, const Mode& mfb) : rgb_fg(rfg), mode_fg(mfg), rgb_bg(rfb), mode_bg(mfb) {}
  rgb  rgb_fg{};
  Mode mode_fg{};
  rgb  rgb_bg{};
  Mode mode_bg{};
};
/*
 * reference colors for converting RGB colors to 4bit colors and vice versa
 */
class Bit4_reference
{
public:
  static rgb BLACK;
  static rgb RED;
  static rgb GREEN;
  static rgb YELLOW;
  static rgb BLUE;
  static rgb MAGENTA;
  static rgb CYAN;
  static rgb WHITE;
  static rgb GRAY;
  static rgb RED_BRIGHT;
  static rgb GREEN_BRIGHT;
  static rgb YELLOW_BRIGHT;
  static rgb BLUE_BRIGHT;
  static rgb MAGENTA_BRIGHT;
  static rgb CYAN_BRIGHT;
  static rgb WHITE_BRIGHT;
  static rgb NONE;
};

// Converts a 4bit color to Term::RGB
rgb bit4_to_rgb(Color4 color);
// Converts a 8bit color to Term::RGB
rgb bit8_to_rgb(std::uint8_t color);
// converts rgb values into Term::RGB
rgb bit24_to_rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b);
// creates an empty rgb color
rgb rgb_empty();

// compares two Term::RGB colors and returns how much they are different
std::uint16_t rgb_compare(rgb rgb_first, rgb rgb_second);

// Converts an RGB color to a 4bit color
Color4  rgb_to_bit4(rgb rgb);
// Converts an RGB color to a 8bit color
uint8_t rgb_to_bit8(rgb rgb);

// checks if the terminal supports RGB (24bit) colors
bool        bit24_support();
// returns ANSI code for 24bit colors, if not supported falls back to 8bit
std::string rgb_to_bit24_auto_fg(rgb color);
// returns ANSI code for 24bit colors, if not supported falls back to 8bit
std::string rgb_to_bit24_auto_bg(rgb color);

// Set the given 4bit color from Term::Color4
std::string color_fg(Color4 color);
// Set the given 4bit / 8bit color
std::string color_fg(std::uint8_t color);
// Set the given 24bit color
std::string color_fg(std::uint8_t r, std::uint8_t g, std::uint8_t b);
// Set the given RGB (24bit) color
std::string color_fg(rgb rgb);
// Set the given foreground color from the RGBF struct
std::string color_fg(rgbf rgbf);
// Set the given foreground color from the RGBF struct with an optional override
std::string color_fg(rgbf rgbf, Mode mode);

// Set the given 4bit color from Term::Color4
std::string color_bg(Color4 color);
// Set the given 4bit / 8bit color
std::string color_bg(std::uint8_t color);
// Set the given 24bit color
std::string color_bg(std::uint8_t r, std::uint8_t g, std::uint8_t b);
// Set the given RGB (24bit) color
std::string color_bg(rgb rgb);
// Set the given background color from the RGBF struct
std::string color_bg(rgbf rgbf);
// Set the given background color from the RGBF struct with an optional override
std::string color_bg(rgbf rgbf, Mode mode);

std::string style(Style style);

// prints the given Term::RGBF color in its original color mode
std::string colorf(rgbf rgbf);

// Create a Term::RGBF color using a 4bit foreground color
rgbf rgbf_fg(Color4 color);
// Create a Term::RGBF color using a 24bit (RGB) foreground color
rgbf rgbf_fg(std::uint8_t r, std::uint8_t g, std::uint8_t b);
// Create a Term::RGBF color using a Term::RGB foreground color
rgbf rgbf_fg(rgb rgb);

// Create a Term::RGBF color using a 4bit background color
rgbf rgbf_bg(Color4 color);
// Create a Term::RGBF color using a 24bit (RGB) background color
rgbf rgbf_bg(std::uint8_t r, std::uint8_t g, std::uint8_t b);
// Create a Term::RGBF color using a Term::RGB background color
rgbf rgbf_bg(rgb rgb);

// Create a Term::RGBF color using a 4bit foreground and background color
rgbf rgbf_fb(Color4 fg, Color4 bg);
// Create a Term::RGBF color using a 24bit (RGB) fore- and background color
rgbf rgbf_fb(std::uint8_t r_fg, std::uint8_t g_fg, std::uint8_t b_fg, std::uint8_t r_bg, std::uint8_t g_bg, std::uint8_t b_bg);
// Create a Term::RGBF color using a Term::RGB fore- and background color
rgbf rgbf_fb(rgb rgb_fg, rgb rgb_bg);

// Create an empty Term::RGBF color for disabling colors completely
rgbf rgbf_empty();

// Print the given Term::RGBF color in Term::Mode::AUTO
std::string color_auto(rgbf rgbf);
// print the given Term::RGBF color in the given color mode
std::string color_auto(rgbf rgbf, Mode mode);

// get the terminal size (row, column) / (Y, X)
std::pair<std::size_t, std::size_t> get_size();
// check if stdin is connected to a TTY
bool                                stdin_connected();
// check if stdout is connected to a TTY
bool                                stdout_connected();
// turn off the cursor
std::string                         cursor_off();
// turn on the cursor
std::string                         cursor_on();
// clear the screen
std::string                         clear_screen();
// clear the screen and the scroll-back buffer
std::string                         clear_buffer();
// move the cursor to the given (row, column) / (Y, X)
std::string                         cursor_move(std::size_t row, std::size_t column);
// move the cursor the given rows up
std::string                         cursor_up(std::size_t rows);
// move the cursor the given rows down
std::string                         cursor_down(std::size_t rows);
// move the cursor the given columns left
std::string                         cursor_left(std::size_t columns);
// move the cursor the given columns right
std::string                         cursor_right(std::size_t columns);
// returns the current cursor position (row, column) (Y, X)
std::pair<std::size_t, std::size_t> cursor_position();
// the ANSI code to generate a cursor position report
std::string                         cursor_position_report();
// clears the screen from the current cursor position to the end of the screen
std::string                         clear_eol();
// save the current terminal state
std::string                         screen_save();
// load a previously saved terminal state
std::string                         screen_load();
// change the title of the terminal, only supported by a few terminals
std::string                         terminal_title(const std::string& title);

}  // namespace Term
