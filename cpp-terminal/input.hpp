#pragma once

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
int read_key();

// If there was a key press, returns the translated key from escape codes,
// otherwise returns 0. If the escape code is not supported, returns a
// negative number.
int read_key0();

}  // namespace Term
