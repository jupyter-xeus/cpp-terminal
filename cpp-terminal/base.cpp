#include <cpp-terminal/base.hpp>
#include <iostream>
#include <string>
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
    return "\x1b[" + std::to_string(row) + ";" + std::to_string(col) + "H";
}

std::string Term::move_cursor_right(int col) {
    return "\x1b[" + std::to_string(col) + "C";
}

std::string Term::move_cursor_down(int row) {
    return "\x1b[" + std::to_string(row) + "B";
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
bool Term::get_term_size(int &rows, int &cols) {
    return Private::get_term_size(rows, cols);
}
