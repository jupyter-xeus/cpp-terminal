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
}  // namespace Term

#endif  // TERMINAL_H
