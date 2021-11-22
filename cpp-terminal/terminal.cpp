#include <cpp-terminal/terminal.h>
#include <cpp-terminal/terminal_base.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cpp-terminal/base.hpp>
#include "private/conversion.hpp"
#include "private/platform.hpp"

Term::Terminal::~Terminal() {
    restore_screen();
}

void Term::Terminal::restore_screen() {
    if (restore_screen_) {
        write("\033[?1049l");  // restore screen
        write(
            "\033"
            "8");  // restore current cursor position
        restore_screen_ = false;
    }
}

void Term::Terminal::save_screen() {
    restore_screen_ = true;
    write(
        "\033"
        "7");              // save current cursor position
    write("\033[?1049h");  // save screen
}



void Term::Terminal::get_cursor_position(int& rows, int& cols) const {
    char buf[32];
    write(cursor_position_report());
    for (unsigned int i = 0; i < sizeof(buf) - 1; i++) {
        while (!Private::read_raw(&buf[i]))
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
            if (Private::convert_string_to_int(&buf[i + 2], "%d;%d", &rows, &cols) !=
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
void Term::Terminal::get_term_size_slow(int& rows, int& cols) {
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
