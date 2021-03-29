#pragma once

#include <string>

namespace Term {
enum class style : unsigned int
 {
    reset = 0,
    bold = 1,
    dim = 2,
    italic = 3,
    underline = 4,
    blink = 5,
    blink_rapid = 6,
    reversed = 7,
    conceal = 8,
    crossed = 9,
    overline = 53
};

enum class fg : unsigned int {
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37,
    reset = 39,
    gray = 90,
    bright_red = 91,
    bright_green = 92,
    bright_yellow = 93,
    bright_blue = 94,
    bright_magenta = 95,
    bright_cyan = 96,
    bright_white = 97
};

enum class bg : unsigned int {
    black = 40,
    red = 41,
    green = 42,
    yellow = 43,
    blue = 44,
    magenta = 45,
    cyan = 46,
    white = 47,
    reset = 49,
    gray = 100,
    bright_red = 101,
    bright_green = 102,
    bright_yellow = 103,
    bright_blue = 104,
    bright_magenta = 105,
    bright_cyan = 106,
    bright_white = 107
};

std::string color(const style& value);
std::string color(const fg& value);
std::string color(const bg& value);

std::string color24_fg(unsigned int, unsigned int, unsigned int);

std::string color24_bg(unsigned int, unsigned int, unsigned int);

void write(const std::string&);

std::string cursor_off();

std::string cursor_on();

std::string clear_screen();

// clears screen + scroll back buffer
std::string clear_screen_buffer();

// If an attempt is made to move the cursor out of the window, the result is
// undefined.
std::string move_cursor(size_t row, size_t col);

// If an attempt is made to move the cursor to the right of the right margin,
// the cursor stops at the right margin.
std::string move_cursor_right(int col);

// If an attempt is made to move the cursor below the bottom margin, the cursor
// stops at the bottom margin.
std::string move_cursor_down(int row);

std::string cursor_position_report();

std::string erase_to_eol();

void restore_screen();

void save_screen();

    // Returns true if the standard input is attached to a terminal
    bool is_stdin_a_tty();

    // Returns true if the standard output is attached to a terminal
    bool is_stdout_a_tty();

}  // namespace Term
