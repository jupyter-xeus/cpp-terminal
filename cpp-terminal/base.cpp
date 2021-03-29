#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <iostream>
#include <stdexcept>
#include "base.hpp"

std::string Term::color(const style& value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}

std::string Term::color(const fg& value) {
    return "\033[" + std::to_string(static_cast<unsigned int>(value)) + 'm';
}

std::string Term::color(const bg& value) {
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

void Term::restore_screen() {
        write("\033[?1049l");  // restore screen
        write("\0338");  // restore current cursor position
}

void Term::save_screen() {
    write("\0337");              // save current cursor position
    write("\033[?1049h");  // save screen
}

    bool Term::is_stdin_a_tty() {
#ifdef _WIN32
        return _isatty(_fileno(stdin));
#else
        return isatty(STDIN_FILENO);
#endif
    }

    bool Term::is_stdout_a_tty() {
#ifdef _WIN32
        return _isatty(_fileno(stdout));
#else
        return isatty(STDOUT_FILENO);
#endif
    }
