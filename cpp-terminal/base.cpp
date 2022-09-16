#include <cpp-terminal/base.hpp>
#include <iostream>
#include <string>
#include "private/conversion.hpp"
#include "private/platform.hpp"

std::string Term::color(style value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}
std::string Term::color(fg value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}
std::string Term::color(bg value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}

std::string Term::color24_fg(unsigned int red,
                             unsigned int green,
                             unsigned int blue) {
    return "\033[38;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

std::string Term::color24_bg(unsigned int red,
                             unsigned int green,
                             unsigned int blue) {
    return "\033[48;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

void Term::write(const std::string& s) {
    std::cout << s << std::flush;
}

std::string Term::cursor_off() {
    return "\x1b[?25l";
}

std::string Term::cursor_on() {
    return "\x1b[?25h";
}

std::string Term::clear_screen() {
    return "\033[2J";
}

std::string Term::clear_screen_buffer() {
    return "\033[3J";
}

std::string Term::move_cursor(size_t row, size_t col) {
    return "\x1b[" + std::to_string(row) + ';' + std::to_string(col) + 'H';
}

std::string Term::move_cursor_right(int col) {
    return "\x1b[" + std::to_string(col) + 'C';
}

std::string Term::move_cursor_down(int row) {
    return "\x1b[" + std::to_string(row) + 'B';
}

std::string Term::cursor_position_report() {
    return "\x1b[6n";
}

std::string Term::erase_to_eol() {
    return "\x1b[K";
}
bool Term::is_stdin_a_tty() {
    return Private::is_stdin_a_tty();
}
bool Term::is_stdout_a_tty() {
    return Private::is_stdout_a_tty();
}
bool Term::get_term_size(int& rows, int& cols) {
    return Private::get_term_size(rows, cols);
}

std::string Term::restore_screen() {
    return "\033[?1049l"  // restores screen
           "\0338";       // restore current cursor position
}

std::string Term::save_screen() {
    return "\0337"         // save current cursor position
           "\033[?1049h";  // save screen
}

void Term::get_cursor_position(int& rows, int& cols) {
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
            if (Private::convert_string_to_int(&buf[i + 2], "%d;%d", &rows,
                                               &cols) != 2) {
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

Term::Terminal::Terminal(bool _clear_screen,
                         bool enable_keyboard,
                         bool disable_signal_keys,
                         bool _hide_cursor)
    : BaseTerminal(enable_keyboard, disable_signal_keys),
      clear_screen{_clear_screen},
      hide_cursor{_hide_cursor} {
    if (clear_screen) {
        write(save_screen() +
              // Fix consoles that ignore save_screen()
              clear_screen_buffer());
    }
    if (hide_cursor)
        write(cursor_off());
}
Term::Terminal::Terminal(bool _clear_screen)
    : BaseTerminal(false, true), clear_screen{_clear_screen} {
    if (clear_screen) {
        write(save_screen()
              // Fix consoles that ignore save_screen()
              + clear_screen_buffer());
    }
}
Term::Terminal::~Terminal() {
    if (clear_screen) {
        // Fix consoles that ignore save_screen()
        write(color(Term::style::reset) + clear_screen_buffer() +
              move_cursor(1, 1) +
              // restores the screen, might be ignored by some terminals
              restore_screen());
    }
    if (hide_cursor)
        write(cursor_on());
}
