#include <cpp-terminal/base.hpp>
#include <iostream>
#include <string>
#include "private/platform.hpp"
#include "private/conversion.hpp"


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

void Term::restore_screen() {
    write("\033[?1049l");  // restore screen
    write(
        "\033"
        "8");  // restore current cursor position
    // restore_screen_ = false;
}

void Term::save_screen() {
    write(
        "\033"
        "7");              // save current cursor position
    write("\033[?1049h");  // save screen
}



void Term::get_cursor_position(int& rows, int& cols){
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
// TODO: repair or remove
// void Term::get_term_size_slow(int& rows, int& cols) {
//     struct CursorOff {
//         const Terminal& term;
//         explicit CursorOff(const Terminal& term) : term{term} {
//             write(cursor_off());
//         }
//         ~CursorOff() { write(cursor_on()); }
//     };
//     CursorOff cursor_off(*this);
//     int old_row{}, old_col{};
//     get_cursor_position(old_row, old_col);
//     write(move_cursor_right(999) + move_cursor_down(999));
//     get_cursor_position(rows, cols);
//     write(move_cursor(old_row, old_col));
// }

Term::Terminal::Terminal(bool enable_keyboard, bool disable_ctrl_c) : BaseTerminal(enable_keyboard, disable_ctrl_c) {
    save_screen();
}
Term::Terminal::Terminal()  : BaseTerminal(false, true)
{
    save_screen();
}
Term::Terminal::~Terminal() {
    restore_screen();
}
