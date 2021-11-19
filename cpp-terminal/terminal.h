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
