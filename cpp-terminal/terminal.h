#ifndef TERMINAL_H
#define TERMINAL_H

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

#include <cpp-terminal/terminal_base.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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
static std::string color(T const& value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}

inline std::string color24_fg(unsigned int red,
                              unsigned int green,
                              unsigned int blue) {
    return "\033[38;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

inline std::string color24_bg(unsigned int red,
                              unsigned int green,
                              unsigned int blue) {
    return "\033[48;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

inline void write(const std::string& s) {
    std::cout << s << std::flush;
}

inline std::string cursor_off() {
    return "\x1b[?25l";
}

inline std::string cursor_on() {
    return "\x1b[?25h";
}

inline std::string clear_screen() {
    return "\033[2J";
}

// clears screen + scroll back buffer
inline std::string clear_screen_buffer() {
    return "\033[3J";
}

// If an attempt is made to move the cursor out of the window, the result is
// undefined.
inline std::string move_cursor(size_t row, size_t col) {
    return "\x1b[" + std::to_string(row) + ";" + std::to_string(col) + "H";
}

// If an attempt is made to move the cursor to the right of the right margin,
// the cursor stops at the right margin.
inline std::string move_cursor_right(int col) {
    return "\x1b[" + std::to_string(col) + "C";
}

// If an attempt is made to move the cursor below the bottom margin, the cursor
// stops at the bottom margin.
inline std::string move_cursor_down(int row) {
    return "\x1b[" + std::to_string(row) + "B";
}

inline std::string cursor_position_report() {
    return "\x1b[6n";
}

inline std::string erase_to_eol() {
    return "\x1b[K";
}

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
    bool restore_screen_;

   public:
    explicit Terminal(bool enable_keyboard = false, bool disable_ctrl_c = true)
        : BaseTerminal(enable_keyboard, disable_ctrl_c),
          restore_screen_{false} {}

    ~Terminal() override { restore_screen(); }

    void restore_screen() {
        if (restore_screen_) {
            write("\033[?1049l");  // restore screen
            write(
                "\033"
                "8");  // restore current cursor position
            restore_screen_ = false;
        }
    }

    void save_screen() {
        restore_screen_ = true;
        write(
            "\033"
            "7");              // save current cursor position
        write("\033[?1049h");  // save screen
    }

    // Waits for a key press, translates escape codes
    int read_key() const {
        int key{};
        while ((key = read_key0()) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return key;
    }

    // If there was a key press, returns the translated key from escape codes,
    // otherwise returns 0. If the escape code is not supported, returns a
    // negative number.
    int read_key0() const {
        char c{};
        if (!read_raw(&c))
            return 0;

        if (c == '\x1b') {
            char seq[4];

            if (!read_raw(&seq[0]))
                return Key::ESC;
            if (!read_raw(&seq[1])) {
                if (seq[0] >= 'a' && seq[0] <= 'z') {
                    // gnome-term, Windows Console
                    return Key::ALT + seq[0];
                }
                if (seq[0] == '\x0d') {
                    // gnome-term
                    return Key::ALT_ENTER;
                }
                return -1;
            }

            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (!read_raw(&seq[2])) {
                        return -2;
                    }
                    if (seq[2] == '~') {
                        switch (seq[1]) {
                            case '1':
                                return Key::HOME;
                            case '2':
                                return Key::INSERT;
                            case '3':
                                return Key::DEL;
                            case '4':
                                return Key::END;
                            case '5':
                                return Key::PAGE_UP;
                            case '6':
                                return Key::PAGE_DOWN;
                            case '7':
                                return Key::HOME;
                            case '8':
                                return Key::END;
                        }
                    } else if (seq[2] == ';') {
                        if (seq[1] == '1') {
                            if (!read_raw(&seq[2])) {
                                return -10;
                            }
                            if (!read_raw(&seq[3])) {
                                return -11;
                            }
                            if (seq[2] == '5') {
                                switch (seq[3]) {
                                    case 'A':
                                        return Key::CTRL_UP;
                                    case 'B':
                                        return Key::CTRL_DOWN;
                                    case 'C':
                                        return Key::CTRL_RIGHT;
                                    case 'D':
                                        return Key::CTRL_LEFT;
                                }
                            }
                            return -12;
                        }
                    } else {
                        if (seq[2] >= '0' && seq[2] <= '9') {
                            if (!read_raw(&seq[3])) {
                                return -3;
                            }
                            if (seq[3] == '~') {
                                if (seq[1] == '1') {
                                    switch (seq[2]) {
                                        case '5':
                                            return Key::F5;
                                        case '7':
                                            return Key::F6;
                                        case '8':
                                            return Key::F7;
                                        case '9':
                                            return Key::F8;
                                    }
                                } else if (seq[1] == '2') {
                                    switch (seq[2]) {
                                        case '0':
                                            return Key::F9;
                                        case '1':
                                            return Key::F10;
                                        case '3':
                                            return Key::F11;
                                        case '4':
                                            return Key::F12;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    switch (seq[1]) {
                        case 'A':
                            return Key::ARROW_UP;
                        case 'B':
                            return Key::ARROW_DOWN;
                        case 'C':
                            return Key::ARROW_RIGHT;
                        case 'D':
                            return Key::ARROW_LEFT;
                        case 'E':
                            return Key::NUMERIC_5;
                        case 'H':
                            return Key::HOME;
                        case 'F':
                            return Key::END;
                    }
                }
            } else if (seq[0] == 'O') {
                switch (seq[1]) {
                    case 'F':
                        return Key::END;
                    case 'H':
                        return Key::HOME;
                    case 'P':
                        return Key::F1;
                    case 'Q':
                        return Key::F2;
                    case 'R':
                        return Key::F3;
                    case 'S':
                        return Key::F4;
                }
            }

            // std::cout << "Unsupported escape sequence:" << std::endl;
            // std::cout << seq[0] << seq[1] << seq[2] << seq[3] << std::endl;
            return -4;
        } else {
            switch (c) {
                case '\x09':  // TAB
                    return Key::TAB;
                case '\x0a':  // LF; falls-through
                case '\x0d':  // CR
                    return Key::ENTER;
                case '\x7f':  // DEL
                    return Key::BACKSPACE;
            }
            if (c == '\xc3') {
                if (!read_raw(&c)) {
                    return -8;
                } else {
                    if (c >= '\xa1' && c <= '\xba') {
                        // xterm
                        return Key::ALT + (c + 'a' - '\xa1');
                    }
                    return -9;
                }
            } else if (c == '\xc2') {
                if (!read_raw(&c)) {
                    return -10;
                } else {
                    if (c == '\x8d') {
                        // xterm
                        return Key::ALT_ENTER;
                    }
                    return -11;
                }
            }
            return c;
        }
    }

    void get_cursor_position(int& rows, int& cols) const {
        char buf[32];
        write(cursor_position_report());
        for (unsigned int i = 0; i < sizeof(buf) - 1; i++) {
            while (!read_raw(&buf[i]))
                ;
            if (buf[i] == 'R') {
                if (i < 5) {
                    throw std::runtime_error(
                        "get_cursor_position(): too short response");
                } else {
                    buf[i] = '\0';
                }
                break;
            }
        }
        // Find the result in the response, drop the rest:
        for (unsigned int i = 0; i < sizeof(buf) - 6; i++) {
            if (buf[i] == '\x1b' && buf[i + 1] == '[') {
                if (convert_string_to_int(&buf[i + 2], "%d;%d", &rows, &cols) !=
                    2) {
                    throw std::runtime_error(
                        "get_cursor_position(): result could not be parsed");
                }
                return;
            }
            if (buf[i] == '\0')
                break;
        }
        throw std::runtime_error(
            "get_cursor_position(): result not found in the response");
    }

    // This function takes about 23ms, so it should only be used as a fallback
    void get_term_size_slow(int& rows, int& cols) const {
        struct CursorOff {
            const Terminal& term;
            explicit CursorOff(const Terminal& term) : term{term} {
                write(cursor_off());
            }
            ~CursorOff() { write(cursor_on()); }
        };
        CursorOff cursor_off(*this);
        int old_row{}, old_col{};
        get_cursor_position(old_row, old_col);
        write(move_cursor_right(999) + move_cursor_down(999));
        get_cursor_position(rows, cols);
        write(move_cursor(old_row, old_col));
    }
};

/*----------------------------------------------------------------------------*/

#define UTF8_ACCEPT 0
#define UTF8_REJECT 0xf

static inline uint8_t utf8_decode_step(uint8_t state,
                                       uint8_t octet,
                                       uint32_t* cpp) {
    static const uint32_t utf8_classtab[0x10] = {
        0x88888888UL, 0x88888888UL, 0x99999999UL, 0x99999999UL,
        0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL,
        0x222222ffUL, 0x22222222UL, 0x22222222UL, 0x22222222UL,
        0x3333333bUL, 0x33433333UL, 0xfff5666cUL, 0xffffffffUL,
    };

    static const uint32_t utf8_statetab[0x10] = {
        0xfffffff0UL, 0xffffffffUL, 0xfffffff1UL, 0xfffffff3UL,
        0xfffffff4UL, 0xfffffff7UL, 0xfffffff6UL, 0xffffffffUL,
        0x33f11f0fUL, 0xf3311f0fUL, 0xf33f110fUL, 0xfffffff2UL,
        0xfffffff5UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL,
    };

    const uint8_t reject = (state >> 3), nonascii = (octet >> 7);
    const uint8_t class_ =
        (!nonascii ? 0
                   : (0xf & (utf8_classtab[(octet >> 3) & 0xf] >>
                             (4 * (octet & 7)))));

    *cpp = (state == UTF8_ACCEPT ? (octet & (0xffU >> class_))
                                 : ((octet & 0x3fU) | (*cpp << 6)));

    return (reject ? 0xf
                   : (0xf & (utf8_statetab[class_] >> (4 * (state & 7)))));
}

/*----------------------------------------------------------------------------*/

inline void codepoint_to_utf8(std::string& s, char32_t c) {
    if (c > 0x0010FFFF) {
        throw std::runtime_error("Invalid UTF32 codepoint.");
    }
    char bytes[4];
    int nbytes = 1;
    char32_t d = c;
    if (c >= 0x10000) {
        nbytes++;
        bytes[3] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x800) {
        nbytes++;
        bytes[2] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x80) {
        nbytes++;
        bytes[1] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    static const unsigned char mask[4] = {0x00, 0xC0, 0xE0, 0xF0};
    bytes[0] = static_cast<char>(d | mask[nbytes - 1]);
    s.append(bytes, nbytes);
}

/*----------------------------------------------------------------------------*/

// Converts an UTF8 string to UTF32.
inline std::u32string utf8_to_utf32(const std::string& s) {
    uint32_t codepoint{};
    uint8_t state = UTF8_ACCEPT;
    std::u32string r{};
    for (char i : s) {
        state = utf8_decode_step(state, i, &codepoint);
        if (state == UTF8_ACCEPT) {
            r.push_back(codepoint);
        } else if (state == UTF8_REJECT) {
            throw std::runtime_error("Invalid byte in UTF8 encoded string");
        }
    }
    if (state != UTF8_ACCEPT) {
        throw std::runtime_error("Expected more bytes in UTF8 encoded string");
    }
    return r;
}

// Converts an UTF32 string to UTF8.
inline std::string utf32_to_utf8(const std::u32string& s) {
    std::string r{};
    for (char32_t i : s) {
        codepoint_to_utf8(r, i);
    }
    return r;
}

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

    char32_t get_char(size_t x, size_t y) {
        return chars[(y - 1) * w + (x - 1)];
    }

    bool get_fg_reset(size_t x, size_t y) {
        return m_fg_reset[(y - 1) * w + (x - 1)];
    }
    bool get_bg_reset(size_t x, size_t y) {
        return m_bg_reset[(y - 1) * w + (x - 1)];
    }
    rgb get_fg(size_t x, size_t y) { return m_fg[(y - 1) * w + (x - 1)]; }
    rgb get_bg(size_t x, size_t y) { return m_bg[(y - 1) * w + (x - 1)]; }

    style get_style(size_t x, size_t y) {
        return m_style[(y - 1) * w + (x - 1)];
    }

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
          m_style(w * h, style::reset) {}

    size_t get_w() const { return w; }

    size_t get_h() const { return h; }

    void set_char(size_t x, size_t y, char32_t c) {
        if (x >= 1 && y >= 1 && x <= w && y <= h) {
            chars[(y - 1) * w + (x - 1)] = c;
        } else {
            throw std::runtime_error("set_char(): (x,y) out of bounds");
        }
    }

    void set_fg_reset(size_t x, size_t y) {
        m_fg_reset[(y - 1) * w + (x - 1)] = true;
        m_fg[(y - 1) * w + (x - 1)] = {256, 256, 256};
    }

    void set_bg_reset(size_t x, size_t y) {
        m_bg_reset[(y - 1) * w + (x - 1)] = true;
        m_fg[(y - 1) * w + (x - 1)] = {256, 256, 256};
    }

    void set_fg(size_t x,
                size_t y,
                unsigned int r,
                unsigned int g,
                unsigned int b) {
        m_fg_reset[(y - 1) * w + (x - 1)] = false;
        m_fg[(y - 1) * w + (x - 1)] = {r, g, b};
    }

    void set_bg(size_t x,
                size_t y,
                unsigned int r,
                unsigned int g,
                unsigned int b) {
        m_bg_reset[(y - 1) * w + (x - 1)] = false;
        m_bg[(y - 1) * w + (x - 1)] = {r, g, b};
    }

    void set_style(size_t x, size_t y, style c) {
        m_style[(y - 1) * w + (x - 1)] = c;
    }

    void set_cursor_pos(int x, int y) {
        cursor_x = x;
        cursor_y = y;
    }

    void set_h(size_t new_h) {
        if (new_h == h) {
            return;
        } else if (new_h > h) {
            size_t dc = (new_h - h) * w;
            chars.insert(chars.end(), dc, ' ');
            m_fg_reset.insert(m_fg_reset.end(), dc, true);
            m_bg_reset.insert(m_bg_reset.end(), dc, true);
            m_fg.insert(m_fg.end(), dc, {0, 0, 0});
            m_bg.insert(m_bg.end(), dc, {0, 0, 0});
            m_style.insert(m_style.end(), dc, style::reset);
            h = new_h;
        } else {
            throw std::runtime_error("Shrinking height not supported.");
        }
    }

    void print_str(int x,
                   int y,
                   const std::string& s,
                   int indent = 0,
                   bool move_cursor = false) {
        std::u32string s2 = utf8_to_utf32(s);
        size_t xpos = x;
        size_t ypos = y;
        for (char32_t i : s2) {
            if (i == U'\n') {
                xpos = x + indent;
                ypos++;
                if (xpos <= w && ypos <= h) {
                    for (int j = 0; j < indent; j++) {
                        set_char(x + j, ypos, '.');
                    }
                } else {
                    // String is out of the window
                    return;
                }
            } else {
                if (xpos <= w && ypos <= h) {
                    set_char(xpos, y, i);
                } else {
                    // String is out of the window
                    return;
                }
                xpos++;
            }
        }
        if (move_cursor) {
            cursor_x = xpos;
            cursor_y = ypos;
        }
    }

    void fill_fg(int x1,
                 int y1,
                 int x2,
                 int y2,
                 unsigned int r,
                 unsigned int g,
                 unsigned int b) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_fg(i, j, r, g, b);
            }
        }
    }

    void fill_bg(int x1,
                 int y1,
                 int x2,
                 int y2,
                 unsigned int r,
                 unsigned int g,
                 unsigned int b) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_bg(i, j, r, g, b);
            }
        }
    }

    void fill_style(int x1, int y1, int x2, int y2, style color) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_style(i, j, color);
            }
        }
    }

    void print_border(bool unicode = true) { print_rect(1, 1, w, h, unicode); }

    void print_rect(size_t x1,
                    size_t y1,
                    size_t x2,
                    size_t y2,
                    bool unicode = true) {
        std::u32string border = utf8_to_utf32("│─┌┐└┘");
        if (unicode) {
            for (size_t j = y1 + 1; j <= y2 - 1; j++) {
                set_char(x1, j, border[0]);
                set_char(x2, j, border[0]);
            }
            for (size_t i = x1 + 1; i <= x2 - 1; i++) {
                set_char(i, y1, border[1]);
                set_char(i, y2, border[1]);
            }
            set_char(x1, y1, border[2]);
            set_char(x2, y1, border[3]);
            set_char(x1, y2, border[4]);
            set_char(x2, y2, border[5]);
        } else {
            for (size_t j = y1 + 1; j <= y2 - 1; j++) {
                set_char(x1, j, '|');
                set_char(x2, j, '|');
            }
            for (size_t i = x1 + 1; i <= x2 - 1; i++) {
                set_char(i, y1, '-');
                set_char(i, y2, '-');
            }
            set_char(x1, y1, '+');
            set_char(x2, y1, '+');
            set_char(x1, y2, '+');
            set_char(x2, y2, '+');
        }
    }

    void clear() {
        for (size_t j = 1; j <= h; j++) {
            for (size_t i = 1; i <= w; i++) {
                set_char(i, j, ' ');
                set_fg_reset(i, j);
                set_bg_reset(i, j);
                set_style(i, j, style::reset);
            }
        }
    }

    static bool rgb_equal(rgb& rgb_one, rgb rgb_two) {
        return rgb_one.r == rgb_two.r && rgb_one.b == rgb_two.b &&
               rgb_one.g == rgb_two.g;
    }

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int x0, int y0, bool term) {
        std::string out;
        if (term) {
            out.append(cursor_off());
        }
        rgb current_fg = {256, 256, 256};
        rgb current_bg = {256, 256, 256};
        bool current_fg_reset = true;
        bool current_bg_reset = true;
        style current_style = style::reset;
        for (size_t j = 1; j <= h; j++) {
            if (term) {
                out.append(move_cursor(y0 + j - 1, x0));
            }
            for (size_t i = 1; i <= w; i++) {
                bool update_fg = false;
                bool update_bg = false;
                bool update_fg_reset = false;
                bool update_bg_reset = false;
                bool update_style = false;
                if (current_fg_reset != get_fg_reset(i, j)) {
                    current_fg_reset = get_fg_reset(i, j);
                    if (current_fg_reset) {
                        update_fg_reset = true;
                        current_fg = {256, 256, 256};
                    }
                }

                if (current_bg_reset != get_bg_reset(i, j)) {
                    current_bg_reset = get_bg_reset(i, j);
                    if (current_bg_reset) {
                        update_bg_reset = true;
                        current_bg = {256, 256, 256};
                    }
                }

                if (current_fg_reset == false) {
                    if (!rgb_equal(current_fg, get_fg(i, j))) {
                        current_fg = get_fg(i, j);
                        update_fg = true;
                    }
                }

                if (current_fg_reset == false) {
                    if (!rgb_equal(current_bg, get_bg(i, j))) {
                        current_bg = get_bg(i, j);
                        update_bg = true;
                    }
                }
                if (current_style != get_style(i, j)) {
                    current_style = get_style(i, j);
                    update_style = true;
                    if (current_style == style::reset) {
                        // style::reset resets fg and bg colors too, we have to
                        // set them again if they are non-default, but if fg or
                        // bg colors are reset, we do not update them, as
                        // style::reset already did that.
                        update_fg = (current_fg_reset == false);
                        update_bg = (current_bg_reset == false);
                    }
                }
                // Set style first, as style::reset will reset colors too
                if (update_style)
                    out.append(color(get_style(i, j)));
                if (update_fg_reset)
                    out.append(color(fg::reset));
                else if (update_fg) {
                    rgb color_tmp = get_fg(i, j);
                    out.append(
                        color24_fg(color_tmp.r, color_tmp.g, color_tmp.b));
                }
                if (update_bg_reset)
                    out.append(color(bg::reset));
                else if (update_bg) {
                    rgb color_tmp = get_bg(i, j);
                    out.append(
                        color24_bg(color_tmp.r, color_tmp.g, color_tmp.b));
                }
                codepoint_to_utf8(out, get_char(i, j));
            }
            if (j < h)
                out.append("\n");
        }
        if (!current_fg_reset)
            out.append(color(fg::reset));
        if (!current_bg_reset)
            out.append(color(bg::reset));
        if (current_style != style::reset)
            out.append(color(style::reset));
        if (term) {
            out.append(move_cursor(y0 + cursor_y - 1, x0 + cursor_x - 1));
            out.append(cursor_on());
        }
        return out;
    };
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
    size_t w, h;                  // width and height of the window
    size_t cursor_x, cursor_y;    // current cursor position
    std::vector<char32_t> chars;  // the characters in row first order
    std::vector<fg> m_fg;
    std::vector<bg> m_bg;
    std::vector<style> m_style;

    char32_t get_char(size_t x, size_t y) {
        return chars[(y - 1) * w + (x - 1)];
    }

    fg get_fg(size_t x, size_t y) { return m_fg[(y - 1) * w + (x - 1)]; }

    bg get_bg(size_t x, size_t y) { return m_bg[(y - 1) * w + (x - 1)]; }

    style get_style(size_t x, size_t y) {
        return m_style[(y - 1) * w + (x - 1)];
    }

   public:
    Window(size_t w, size_t h)
        : w{w},
          h{h},
          cursor_x{1},
          cursor_y{1},
          chars(w * h, ' '),
          m_fg(w * h, fg::reset),
          m_bg(w * h, bg::reset),
          m_style(w * h, style::reset) {}

    size_t get_w() const { return w; }

    size_t get_h() const { return h; }

    void set_char(size_t x, size_t y, char32_t c) {
        if (x >= 1 && y >= 1 && x <= w && y <= h) {
            chars[(y - 1) * w + (x - 1)] = c;
        } else {
            throw std::runtime_error("set_char(): (x,y) out of bounds");
        }
    }

    void set_fg(size_t x, size_t y, fg c) { m_fg[(y - 1) * w + (x - 1)] = c; }

    void set_bg(size_t x, size_t y, bg c) { m_bg[(y - 1) * w + (x - 1)] = c; }

    void set_style(size_t x, size_t y, style c) {
        m_style[(y - 1) * w + (x - 1)] = c;
    }

    void set_cursor_pos(int x, int y) {
        cursor_x = x;
        cursor_y = y;
    }

    void set_h(size_t new_h) {
        if (new_h == h) {
            return;
        } else if (new_h > h) {
            size_t dc = (new_h - h) * w;
            chars.insert(chars.end(), dc, ' ');
            m_fg.insert(m_fg.end(), dc, fg::reset);
            m_bg.insert(m_bg.end(), dc, bg::reset);
            m_style.insert(m_style.end(), dc, style::reset);
            h = new_h;
        } else {
            throw std::runtime_error("Shrinking height not supported.");
        }
    }

    void print_str(int x,
                   int y,
                   const std::string& s,
                   int indent = 0,
                   bool move_cursor = false) {
        std::u32string s2 = utf8_to_utf32(s);
        size_t xpos = x;
        size_t ypos = y;
        for (char32_t i : s2) {
            if (i == U'\n') {
                xpos = x + indent;
                ypos++;
                if (xpos <= w && ypos <= h) {
                    for (int j = 0; j < indent; j++) {
                        set_char(x + j, ypos, '.');
                    }
                } else {
                    // String is out of the window
                    return;
                }
            } else {
                if (xpos <= w && ypos <= h) {
                    set_char(xpos, y, i);
                } else {
                    // String is out of the window
                    return;
                }
                xpos++;
            }
        }
        if (move_cursor) {
            cursor_x = xpos;
            cursor_y = ypos;
        }
    }

    void fill_fg(int x1, int y1, int x2, int y2, fg color) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_fg(i, j, color);
            }
        }
    }

    void fill_bg(int x1, int y1, int x2, int y2, bg color) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_bg(i, j, color);
            }
        }
    }

    void fill_style(int x1, int y1, int x2, int y2, style color) {
        for (int j = y1; j <= y2; j++) {
            for (int i = x1; i <= x2; i++) {
                set_style(i, j, color);
            }
        }
    }

    void print_border(bool unicode = true) { print_rect(1, 1, w, h, unicode); }

    void print_rect(size_t x1,
                    size_t y1,
                    size_t x2,
                    size_t y2,
                    bool unicode = true) {
        std::u32string border = utf8_to_utf32("│─┌┐└┘");
        if (unicode) {
            for (size_t j = y1 + 1; j <= y2 - 1; j++) {
                set_char(x1, j, border[0]);
                set_char(x2, j, border[0]);
            }
            for (size_t i = x1 + 1; i <= x2 - 1; i++) {
                set_char(i, y1, border[1]);
                set_char(i, y2, border[1]);
            }
            set_char(x1, y1, border[2]);
            set_char(x2, y1, border[3]);
            set_char(x1, y2, border[4]);
            set_char(x2, y2, border[5]);
        } else {
            for (size_t j = y1 + 1; j <= y2 - 1; j++) {
                set_char(x1, j, '|');
                set_char(x2, j, '|');
            }
            for (size_t i = x1 + 1; i <= x2 - 1; i++) {
                set_char(i, y1, '-');
                set_char(i, y2, '-');
            }
            set_char(x1, y1, '+');
            set_char(x2, y1, '+');
            set_char(x1, y2, '+');
            set_char(x2, y2, '+');
        }
    }

    void clear() {
        for (size_t j = 1; j <= h; j++) {
            for (size_t i = 1; i <= w; i++) {
                set_char(i, j, ' ');
                set_fg(i, j, fg::reset);
                set_bg(i, j, bg::reset);
                set_style(i, j, style::reset);
            }
        }
    }

    // TODO: add Window/Screen parameter here, to be used like this:
    // old_scr = scr;
    // scr.print_str(...)
    // scr.render(1, 1, old_scr)
    std::string render(int x0, int y0, bool term) {
        std::string out;
        if (term) {
            out.append(cursor_off());
        }
        fg current_fg = fg::reset;
        bg current_bg = bg::reset;
        style current_style = style::reset;
        for (size_t j = 1; j <= h; j++) {
            if (term) {
                out.append(move_cursor(y0 + j - 1, x0));
            }
            for (size_t i = 1; i <= w; i++) {
                bool update_fg = false;
                bool update_bg = false;
                bool update_style = false;
                if (current_fg != get_fg(i, j)) {
                    current_fg = get_fg(i, j);
                    update_fg = true;
                }
                if (current_bg != get_bg(i, j)) {
                    current_bg = get_bg(i, j);
                    update_bg = true;
                }
                if (current_style != get_style(i, j)) {
                    current_style = get_style(i, j);
                    update_style = true;
                    if (current_style == style::reset) {
                        // style::reset resets fg and bg colors too, we have to
                        // set them again if they are non-default, but if fg or
                        // bg colors are reset, we do not update them, as
                        // style::reset already did that.
                        update_fg = (current_fg != fg::reset);
                        update_bg = (current_bg != bg::reset);
                    }
                }
                // Set style first, as style::reset will reset colors too
                if (update_style)
                    out.append(color(get_style(i, j)));
                if (update_fg)
                    out.append(color(get_fg(i, j)));
                if (update_bg)
                    out.append(color(get_bg(i, j)));
                codepoint_to_utf8(out, get_char(i, j));
            }
            if (j < h)
                out.append("\n");
        }
        if (current_fg != fg::reset)
            out.append(color(fg::reset));
        if (current_bg != bg::reset)
            out.append(color(bg::reset));
        if (current_style != style::reset)
            out.append(color(style::reset));
        if (term) {
            out.append(move_cursor(y0 + cursor_y - 1, x0 + cursor_x - 1));
            out.append(cursor_on());
        }
        return out;
    };
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

static std::string concat(const std::vector<std::string>& lines) {
    std::string s;
    for (auto& line : lines) {
        s.append(line + "\n");
    }
    return s;
}

static std::vector<std::string> split(const std::string& s) {
    size_t j = 0;
    std::vector<std::string> lines;
    lines.emplace_back("");
    if (s[s.size() - 1] != '\n')
        throw std::runtime_error("\\n is required");
    for (size_t i = 0; i < s.size() - 1; i++) {
        if (s[i] == '\n') {
            j++;
            lines.emplace_back("");
        } else {
            lines[j].push_back(s[i]);
        }
    }
    return lines;
}

static char32_t U(const std::string& s) {
    std::u32string s2 = Term::utf8_to_utf32(s);
    if (s2.size() != 1)
        throw std::runtime_error("U(s): s not a codepoint.");
    return s2[0];
}

static void print_left_curly_bracket(Term::Window& scr, int x, int y1, int y2) {
    int h = y2 - y1 + 1;
    if (h == 1) {
        scr.set_char(x, y1, U("]"));
    } else {
        scr.set_char(x, y1, U("┐"));
        for (int j = y1 + 1; j <= y2 - 1; j++) {
            scr.set_char(x, j, U("│"));
        }
        scr.set_char(x, y2, U("┘"));
    }
}

static void render(Term::Window& scr, const Model& m, size_t cols) {
    scr.clear();
    print_left_curly_bracket(scr, cols, 1, m.lines.size());
    scr.print_str(
        cols - 6, m.lines.size(),
        std::to_string(m.cursor_row) + "," + std::to_string(m.cursor_col));
    for (size_t j = 0; j < m.lines.size(); j++) {
        if (j == 0) {
            scr.print_str(1, j + 1, m.prompt_string);
            scr.fill_fg(1, j + 1, m.prompt_string.size(), m.lines.size(),
                        Term::fg::green);
            scr.fill_style(1, j + 1, m.prompt_string.size(), m.lines.size(),
                           Term::style::bold);
        } else {
            for (size_t i = 0; i < m.prompt_string.size() - 1; i++) {
                scr.set_char(i + 1, j + 1, '.');
            }
        }
        scr.print_str(m.prompt_string.size() + 1, j + 1, m.lines[j]);
    }
    scr.set_cursor_pos(m.prompt_string.size() + m.cursor_col, m.cursor_row);
}

inline std::string prompt(const Terminal& term,
                          const std::string& prompt_string,
                          std::vector<std::string>& history,
                          std::function<bool(std::string)>& iscomplete) {
    int row, col;
    bool term_attached = Terminal::is_stdin_a_tty();
    if (term_attached) {
        term.get_cursor_position(row, col);
    } else {
        row = 1;
        col = 1;
    }
    int rows, cols;
    if (!term.get_term_size(rows, cols)) {
        rows = 25;
        cols = 80;
    }

    Model m;
    m.prompt_string = prompt_string;
    m.lines.emplace_back("");
    m.cursor_col = 1;
    m.cursor_row = 1;

    // Make a local copy of history that can be modified by the user. All
    // changes will be forgotten once a command is submitted.
    std::vector<std::string> hist = history;
    size_t history_pos = hist.size();
    hist.push_back(concat(m.lines));  // Push back empty input

    Term::Window scr(cols, 1);
    int key;
    render(scr, m, cols);
    std::cout << scr.render(1, row, term_attached) << std::flush;
    bool not_complete = true;
    while (not_complete) {
        key = term.read_key();
        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
            (!iscntrl(key) && key < 128)) {
            std::string before =
                m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
            std::string newchar;
            newchar.push_back(key);
            std::string after =
                m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
            m.lines[m.cursor_row - 1] = before += newchar += after;
            m.cursor_col++;
        } else if (key == Key::CTRL + 'd') {
            if (m.lines.size() == 1 && m.lines[m.cursor_row - 1].empty()) {
                m.lines[m.cursor_row - 1].push_back(Key::CTRL + 'd');
                std::cout << "\n" << std::flush;
                history.push_back(m.lines[0]);
                return m.lines[0];
            }
        } else {
            switch (key) {
                case Key::BACKSPACE:
                    if (m.cursor_col > 1) {
                        std::string before = m.lines[m.cursor_row - 1].substr(
                            0, m.cursor_col - 2);
                        std::string after =
                            m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
                        m.lines[m.cursor_row - 1] = before + after;
                        m.cursor_col--;
                    } else if (m.cursor_col == 1 && m.cursor_row > 1) {
                        m.cursor_col = m.lines[m.cursor_row - 2].size() + 1;
                        m.lines[m.cursor_row - 2] += m.lines[m.cursor_row - 1];
                        m.lines.erase(m.lines.begin() + m.cursor_row - 1);
                        m.cursor_row--;
                    }
                    break;
                case Key::DEL:
                    if (m.cursor_col <= m.lines[m.cursor_row - 1].size()) {
                        std::string before = m.lines[m.cursor_row - 1].substr(
                            0, m.cursor_col - 1);
                        std::string after =
                            m.lines[m.cursor_row - 1].substr(m.cursor_col);
                        m.lines[m.cursor_row - 1] = before + after;
                    }
                    break;
                case Key::ARROW_LEFT:
                    if (m.cursor_col > 1) {
                        m.cursor_col--;
                    }
                    break;
                case Key::ARROW_RIGHT:
                    if (m.cursor_col <= m.lines[m.cursor_row - 1].size()) {
                        m.cursor_col++;
                    }
                    break;
                case Key::HOME:
                    m.cursor_col = 1;
                    break;
                case Key::END:
                    m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                    break;
                case Key::ARROW_UP:
                    if (m.cursor_row == 1) {
                        if (history_pos > 0) {
                            hist[history_pos] = concat(m.lines);
                            history_pos--;
                            m.lines = split(hist[history_pos]);
                            m.cursor_row = m.lines.size();
                            if (m.cursor_col >
                                m.lines[m.cursor_row - 1].size() + 1) {
                                m.cursor_col =
                                    m.lines[m.cursor_row - 1].size() + 1;
                            }
                            if (m.lines.size() > scr.get_h()) {
                                scr.set_h(m.lines.size());
                            }
                        }
                    } else {
                        m.cursor_row--;
                        if (m.cursor_col >
                            m.lines[m.cursor_row - 1].size() + 1) {
                            m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                        }
                    }
                    break;
                case Key::ARROW_DOWN:
                    if (m.cursor_row == m.lines.size()) {
                        if (history_pos < hist.size() - 1) {
                            hist[history_pos] = concat(m.lines);
                            history_pos++;
                            m.lines = split(hist[history_pos]);
                            m.cursor_row = 1;
                            if (m.cursor_col >
                                m.lines[m.cursor_row - 1].size() + 1) {
                                m.cursor_col =
                                    m.lines[m.cursor_row - 1].size() + 1;
                            }
                            if (m.lines.size() > scr.get_h()) {
                                scr.set_h(m.lines.size());
                            }
                        }
                    } else {
                        m.cursor_row++;
                        if (m.cursor_col >
                            m.lines[m.cursor_row - 1].size() + 1) {
                            m.cursor_col = m.lines[m.cursor_row - 1].size() + 1;
                        }
                    }
                    break;
                case Key::ENTER:
                    not_complete = !iscomplete(concat(m.lines));
                    if (not_complete) {
                        key = Key::ALT_ENTER;
                    } else {
                        break;
                    }
                    [[fallthrough]];
                case Key::CTRL + 'n':
                case Key::ALT_ENTER:
                    std::string before =
                        m.lines[m.cursor_row - 1].substr(0, m.cursor_col - 1);
                    std::string after =
                        m.lines[m.cursor_row - 1].substr(m.cursor_col - 1);
                    m.lines[m.cursor_row - 1] = before;
                    if (m.cursor_row < m.lines.size()) {
                        // Not at the bottom row, can't push back
                        m.lines.insert(m.lines.begin() + m.cursor_row, after);
                    } else {
                        m.lines.push_back(after);
                    }
                    m.cursor_col = 1;
                    m.cursor_row++;
                    if (m.lines.size() > scr.get_h()) {
                        scr.set_h(m.lines.size());
                    }
            }
        }
        render(scr, m, cols);
        std::cout << scr.render(1, row, term_attached) << std::flush;
        if (row + (int)scr.get_h() - 1 > rows) {
            row = rows - ((int)scr.get_h() - 1);
            std::cout << scr.render(1, row, term_attached) << std::flush;
        }
    }
    // REPL currently supports pressing enter at any cursor position, so
    // correctly draw if the user presses enter at a line other than the bottom
    std::string line_skips;
    for (size_t i = 0; i <= m.lines.size() - m.cursor_row; i++) {
        line_skips += "\n";
    }
    std::cout << line_skips << std::flush;
    history.push_back(concat(m.lines));
    return concat(m.lines);
}

}  // namespace Term

#endif  // TERMINAL_H
