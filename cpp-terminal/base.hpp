#pragma once

#include <string>
// TODO: remove include
#include "private/platform.hpp"

namespace Term {

enum class style {
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

enum class fg {
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

enum class bg {
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

std::string color(style);
std::string color(fg);
std::string color(bg);

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
std::string move_cursor(size_t, size_t);

// If an attempt is made to move the cursor to the right of the right margin,
// the cursor stops at the right margin.
std::string move_cursor_right(int);

// If an attempt is made to move the cursor below the bottom margin, the cursor
// stops at the bottom margin.
std::string move_cursor_down(int);

std::string cursor_position_report();

std::string erase_to_eol();

bool is_stdin_a_tty();
bool is_stdout_a_tty();
bool get_term_size(int&, int&);

void restore_screen();

void save_screen();

void get_cursor_position(int&, int&);

// initializes the terminal
class Terminal : public Private::BaseTerminal {
   private:
    bool clear_screen{};

   public:
    Terminal(bool _clear_screen, bool enable_keyboard, bool disable_ctrl_c);
    // providing no parameters will disable the keyboard and ctrl+c
    Terminal(bool _clear_screen);

    virtual ~Terminal() override;
};

}  // namespace Term
