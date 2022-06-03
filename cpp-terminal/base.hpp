#pragma once

#include <string>
#include "private/platform.hpp"

namespace Term {

// codes for styling terminal output, to be used with color()
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

// codes for the foreground color, to be used with color()
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

// codes for the background color, to be used with color()
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

// represents a RGB color
struct RGB {
    uint8_t r, g, b;
};

// used as reference values for converting 4bit colors to RGB
// the default values can be changed if wished
struct bit4_reference {
    RGB black{0,0,0};
    RGB red{151, 12, 40};
    RGB green{1, 142, 66};
    RGB yellow{238, 198, 67};
    RGB blue{13, 33, 161};
    RGB magenta{255, 0, 144};
    RGB cyan{0, 159, 184};
    RGB white{240,240,240};
    RGB gray{127,127,127};
    RGB bright_red{241, 85, 116};
    RGB bright_green{52, 254, 146};
    RGB bright_yellow{243, 215, 124};
    RGB bright_blue{63, 136, 197};
    RGB bright_magenta{255, 92, 184};
    RGB bright_cyan{51, 228, 255};
    RGB bright_white{255, 255, 255};
};

// returns the given style code as ANSI escape code
std::string color(style);
// returns the given foreground color as ANSI escape code
std::string color(fg);
// returns the given background color as ANSI escape code
std::string color(bg);
// returns the given 8bit color as ANSI escape code
std::string color8_fg(uint8_t);
// returns the given 8bit color as ANSI escape code
std::string color8_bg(uint8_t);
// returns the given RGB color as ANSI escape code
std::string color24_fg(unsigned int, unsigned int, unsigned int);
// returns the given RGB color as ANSI escape code
std::string color24_fg(RGB color);
// returns the given RGB color as ANSI escape code
std::string color24_bg(unsigned int, unsigned int, unsigned int);
// returns the given RGB color as ANSI escape code
std::string color24_bg(RGB color);

// converts 24bit colors to Term::RGB
RGB bit24_to_rgb(uint8_t r, uint8_t g, uint8_t b);
// converts 8bit colors to Term::
// TODO
RGB bit8_to_rgb(uint8_t color);
// converts 8bit colors to Term::RGB
// TODO
RGB bit8_to_rgb(uint8_t color, bit4_reference color_ref);
// converts 4bit colors to Term::RGB
RGB bit4_to_rgb(fg color, bit4_reference color_ref);
// converts 4bit colors to Term::RGB
RGB bit4_to_rgb(fg color);
// converts 4bit colors to Term::RGB
RGB bit4_to_rgb(bg color, bit4_reference color_ref);
// converts 4bit colors to Term::RGB
RGB bit4_to_rgb(bg color);

// Generates a diff from two colors. The lower the result the closer the color
uint16_t rgb_diff(RGB color_first, RGB color_second);

// converts Term::RGB to 8bit colors
uint8_t rgb_to_bit8(RGB color);
// converts Term::RGB to 8bit colors
//uint8_t rgb_to_bit8(RGB color);
// converts Term::RGB to 4bit colors foreground
fg rgb_to_bit4_fg(RGB color);
// converts Term::RGB to 4bit colors foreground
fg rgb_to_bit4_fg(RGB color, bit4_reference color_ref);
// converts Term::RGB to 4bit colors foreground
bg rgb_to_bit4_bg(RGB color);
// converts Term::RGB to 4bit colors foreground
bg rgb_to_bit4_bg(RGB color, bit4_reference color_ref);

// writes a string to the console and flushes stdout
void write(const std::string&);

// escape code to deactivate the cursor
std::string cursor_off();
// escape code to enable the cursor
std::string cursor_on();
// escape code to clear the screen
std::string clear_screen();
// escape code to clear the screen + scroll back buffer
std::string clear_screen_buffer();
// move the cursor to the given place in the screen, counting starts at 1
// If an attempt is made to move the cursor out of the window, the result is
// undefined.
std::string move_cursor(size_t, size_t);
// move the cursor the given number of columns right
// If an attempt is made to move the cursor to the right of the right margin,
// the cursor stops at the right margin.
std::string move_cursor_right(int);
// move the cursor the given number of rows down
// If an attempt is made to move the cursor below the bottom margin, the cursor
// stops at the bottom margin.
std::string move_cursor_down(int);

// returns the current cursor position
// used for determining the screen size
std::string cursor_position_report();
// Erases part of the line. If n is 0 (or missing),
// clear from cursor to the end of the line.
// If n is 1, clear from cursor to beginning of the line.
// If n is 2, clear entire line. Cursor position does not change.
std::string erase_to_eol();
// returns true if stdin is connected to a console
// if false, the program can't catch user input
bool is_stdin_a_tty();
// returns false if stdout is connected to a console
// if false, the program can't use raw mode
bool is_stdout_a_tty();
// returns the terminal size in rows and columns
// returning false means that the program isn't connected to a terminal
// and therefore using raw mode isn't possible
bool get_term_size(int&, int&);

// escape code for saving the current terminal state
std::string save_screen();
// escape code for restoring a saved terminal state
std::string restore_screen();

// returns the current position of the cursor
void get_cursor_position(int&, int&);

// initializes and prepares the terminal
class Terminal : public Private::BaseTerminal {
   private:
    bool clear_screen{};
    bool hide_cursor{};

   public:
    // prepares the terminal in various ways
    Terminal(bool _clear_screen,
             bool enable_keyboard,
             bool disable_ctrl_c,
             bool _hide_cursor);
    // providing no parameters will disable the keyboard and ctrl+c
    explicit Terminal(bool _clear_screen);

    ~Terminal() override;
};

}  // namespace Term
