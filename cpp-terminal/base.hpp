#pragma once

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/platforms/macros.hpp"
#include "cpp-terminal/platforms/platform.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

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

std::string style(Style style);

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
