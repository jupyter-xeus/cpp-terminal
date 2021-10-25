#ifndef TERMINAL_H
#define TERMINAL_H

#include <cpp-terminal/terminal_base.h>
#include <functional>
#include <string>
#include <vector>

/* This file is all platform independent, it contains the logic to build
 * the features that users need in a terminal application.
 *
 * The ANSI escape sequences used here are supported by all terminals (Linux,
 * macOS, Windows). All the functionality here must work on all platforms. The
 * Windows terminal is probably the most limiting, and so we restrict to the
 * capabilities that it supports, as documented at:
 *
 * https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */

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

template <typename T>
std::string color(T const&);

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

enum Key {
    BACKSPACE = 1000,
    ENTER,
    ALT_ENTER,
    TAB,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_UP,
    CTRL_DOWN,
    CTRL_RIGHT,
    CTRL_LEFT,
    NUMERIC_5,
    DEL,
    HOME,
    INSERT,
    END,
    PAGE_UP,
    PAGE_DOWN,
    ESC,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    // special keys
    CTRL = -96,
    ALT = -128
};

class Terminal : public BaseTerminal {
    bool restore_screen_ = false;

   public:
    Terminal(bool enable_keyboard, bool disable_ctrl_c)
        : BaseTerminal(enable_keyboard, disable_ctrl_c){};
    // providing no parameters will disable the keyboard and ctrl+c
    Terminal() : BaseTerminal(false, true){};

    virtual ~Terminal() override;

    void restore_screen();

    void save_screen();

    // Waits for a key press, translates escape codes
    int read_key() const;

    // If there was a key press, returns the translated key from escape codes,
    // otherwise returns 0. If the escape code is not supported, returns a
    // negative number.
    int read_key0() const;

    void get_cursor_position(int&, int&) const;

    // This function takes about 23ms, so it should only be used as a fallback
    void get_term_size_slow(int&, int&);
};

// Converts an UTF8 string to UTF32.
std::u32string utf8_to_utf32(const std::string&);

// Converts an UTF32 string to UTF8.
std::string utf32_to_utf8(const std::u32string&);

/* Represents a rectangular window, as a 2D array of characters and their
 * attributes. The render method can convert this internal representation to a
 * string that when printed will show the Window on the screen.
 *
 * Note: the characters are represented by char32_t, representing their UTF-32
 * code point. The natural way to represent a character in a terminal would be
 * a "unicode grapheme cluster", but due to a lack of a good library for C++
 * that could handle those, we simply use a Unicode code point as a character.
 */
class Window_24bit {
   private:
    size_t w, h;                  // width and height of the window
    size_t cursor_x, cursor_y;    // current cursor position
    std::vector<char32_t> chars;  // the characters in row first order
    struct rgb {
        unsigned int r, g, b;
    };
    std::vector<rgb> m_fg;
    std::vector<rgb> m_bg;
    std::vector<bool> m_fg_reset;
    std::vector<bool> m_bg_reset;
    std::vector<style> m_style;

    char32_t get_char(size_t, size_t);

    bool get_fg_reset(size_t, size_t);
    bool get_bg_reset(size_t, size_t);
    rgb get_fg(size_t, size_t);
    rgb get_bg(size_t, size_t);

    style get_style(size_t, size_t);

   public:
    Window_24bit(size_t w, size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, {0, 0, 0}),
          m_bg(w * h, {0, 0, 0}),
          m_fg_reset(w * h, true),
          m_bg_reset(w * h, true),
          m_style(w * h, style::reset){};

    size_t get_w();

    size_t get_h();

    void set_char(size_t, size_t, char32_t);

    void set_fg_reset(size_t, size_t);

    void set_bg_reset(size_t, size_t);

    void set_fg(size_t, size_t, unsigned int, unsigned int, unsigned int);

    void set_bg(size_t, size_t, unsigned int, unsigned int, unsigned int);

    void set_style(size_t, size_t, style);

    void set_cursor_pos(int, int);

    void set_h(size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_bg(int, int, int, int, unsigned int, unsigned int, unsigned int);

    void fill_style(int, int, int, int, style);

    void print_border(bool = true);

    void print_rect(size_t, size_t, size_t, size_t, bool = true);

    void clear();

    bool rgb_equal(rgb&, rgb);

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int, int, bool);
};

/* Represents a rectangular window, as a 2D array of characters and their
 * attributes. The render method can convert this internal representation to a
 * string that when printed will show the Window on the screen.
 *
 * Note: the characters are represented by char32_t, representing their UTF-32
 * code point. The natural way to represent a character in a terminal would be
 * a "unicode grapheme cluster", but due to a lack of a good library for C++
 * that could handle those, we simply use a Unicode code point as a character.
 */
class Window {
   private:
    size_t w{}, h{};              // width and height of the window
    size_t cursor_x, cursor_y;    // current cursor position
    std::vector<char32_t> chars;  // the characters in row first order
    std::vector<fg> m_fg;
    std::vector<bg> m_bg;
    std::vector<style> m_style;

    char32_t get_char(size_t, size_t);

    fg get_fg(size_t, size_t);

    bg get_bg(size_t, size_t);

    style get_style(size_t, size_t);

   public:
    Window(size_t w, size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, fg::reset),
          m_bg(w * h, bg::reset),
          m_style(w * h, style::reset){};

    size_t get_w();

    size_t get_h();

    void set_char(size_t, size_t, char32_t);

    void set_fg(size_t, size_t, fg);

    void set_bg(size_t, size_t, bg);

    void set_style(size_t, size_t, style);

    void set_cursor_pos(int, int);

    void set_h(size_t);

    void print_str(int, int, const std::string&, int = 0, bool = false);

    void fill_fg(int, int, int, int, fg);

    void fill_bg(int, int, int, int, bg);

    void fill_style(int, int, int, int, style);

    void print_border(bool);

    void print_rect(size_t, size_t, size_t, size_t, bool = true);

    void clear();

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int, int, bool);
};

// This model contains all the information about the state of the prompt in an
// abstract way, irrespective of where or how it is rendered.
struct Model {
    std::string prompt_string;  // The string to show as the prompt
    std::vector<std::string>
        lines;  // The current input string in the prompt as
                // a vector of lines, without '\n' at the end.
    // The current cursor position in the "input" string, starting from (1,1)
    size_t cursor_col{}, cursor_row{};
};

std::string concat(const std::vector<std::string>&);

std::vector<std::string> split(const std::string&);

char32_t U(const std::string&);

void print_left_curly_bracket(Term::Window&, int, int, int);

void render(Term::Window&, const Model&, size_t);

std::string prompt(Terminal&,
                   const std::string&,
                   std::vector<std::string>&,
                   std::function<bool(std::string)>&);

// UTF8
uint8_t utf8_decode_step(uint8_t, uint8_t, uint32_t*);

void codepoint_to_utf8(std::string&, char32_t);

std::u32string utf8_to_utf32(const std::string&);
std::string utf32_to_utf8(const std::u32string&);

}  // namespace Term

#endif  // TERMINAL_H
