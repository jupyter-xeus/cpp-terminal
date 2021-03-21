// terminal base

#pragma once

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <iostream>

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
    return "\x1b[" + std::to_string(row) + ';' + std::to_string(col) + 'H';
}

// If an attempt is made to move the cursor to the right of the right margin,
// the cursor stops at the right margin.
inline std::string move_cursor_right(int col) {
    return "\x1b[" + std::to_string(col) + 'C';
}

// If an attempt is made to move the cursor below the bottom margin, the cursor
// stops at the bottom margin.
inline std::string move_cursor_down(int row) {
    return "\x1b[" + std::to_string(row) + 'B';
}

inline std::string cursor_position_report() {
    return "\x1b[6n";
}

inline std::string erase_to_eol() {
    return "\x1b[K";
}

void restore_screen() {
        write("\033[?1049l");  // restore screen
        write("\0338");  // restore current cursor position
}

void save_screen() {
    write("\0337");              // save current cursor position
    write("\033[?1049h");  // save screen
}

    bool get_term_size(int& rows, int& cols) {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO inf;
        if (GetConsoleScreenBufferInfo(hout, &inf)) {
            cols = inf.srWindow.Right - inf.srWindow.Left + 1;
            rows = inf.srWindow.Bottom - inf.srWindow.Top + 1;
            return true;
        } else {
            // This happens when we are not connected to a terminal
            return false;
        }
#else
        struct winsize ws {};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
            // This happens when we are not connected to a terminal
            return false;
        } else {
            cols = ws.ws_col;
            rows = ws.ws_row;
            return true;
        }
#endif
    }

    // Returns true if the standard input is attached to a terminal
    [[maybe_unused]]static bool is_stdin_a_tty() {
#ifdef _WIN32
        return _isatty(_fileno(stdin));
#else
        return isatty(STDIN_FILENO);
#endif
    }

    // Returns true if the standard output is attached to a terminal
    [[maybe_unused]]static bool is_stdout_a_tty() {
#ifdef _WIN32
        return _isatty(_fileno(stdout));
#else
        return isatty(STDOUT_FILENO);
#endif
    }

}  // namespace Term
