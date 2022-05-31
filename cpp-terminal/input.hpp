#pragma once
#include <string>

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

// Waits for a key press, translates escape codes
// if Term:Terminal is not enabling the keyboard it'll loop for infinity
int read_key();

// If there was a key press, returns the translated key from escape codes,
// otherwise returns 0. If the escape code is not supported it returns a
// negative number.
// if Term::Terminal is not enabling the keyboard it'll always return 0
int read_key0();

// returns the stdin as a string
// waits until the EOT signal is send
// if Term::Terminal is not enabling the keyboard this function will wait until
// the user presses CTRL+D (which sends the EOT signal)
std::string read_stdin();

// returns stdin as a string, Term::Terminal is used to enable input to make the
// function non-blocking, use Term::read_stdin() when Term::Terminal is already
// created
std::string read_stdin_alone();

}  // namespace Term
