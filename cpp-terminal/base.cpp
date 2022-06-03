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

std::string Term::color8_fg(uint8_t color) {
    return "\033[38;5;" + std::to_string(color) + 'm';
}

std::string Term::color8_bg(uint8_t color) {
    return "\033[48;5;" + std::to_string(color) + 'm';
}

std::string Term::color24_fg(unsigned int red,
                             unsigned int green,
                             unsigned int blue) {
    return "\033[38;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

std::string Term::color24_fg(RGB color) {
    return "\033[38;2;" + std::to_string(color.r) + ';' + std::to_string(color.g) +
           ';' + std::to_string(color.b) + 'm';
}

std::string Term::color24_bg(unsigned int red,
                             unsigned int green,
                             unsigned int blue) {
    return "\033[48;2;" + std::to_string(red) + ';' + std::to_string(green) +
           ';' + std::to_string(blue) + 'm';
}

std::string Term::color24_bg(RGB color) {
    return "\033[48;2;" + std::to_string(color.r) + ';' + std::to_string(color.g) +
           ';' + std::to_string(color.b) + 'm';
}

Term::RGB Term::bit24_to_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return RGB{r, g, b};
}
Term::RGB Term::bit8_to_rgb(uint8_t color, Term::bit4_reference color_ref) {
    // convert 4bit colors
    if (color <= 15) {
        switch (color) {
            case 0:
                return bit4_to_rgb(fg::black, color_ref);
            case 1:
                return bit4_to_rgb(fg::red, color_ref);
            case 2:
                return bit4_to_rgb(fg::green, color_ref);
            case 3:
                return bit4_to_rgb(fg::yellow, color_ref);
            case 4:
                return bit4_to_rgb(fg::blue, color_ref);
            case 5:
                return bit4_to_rgb(fg::magenta, color_ref);
            case 6:
                return bit4_to_rgb(fg::cyan, color_ref);
            case 7:
                return bit4_to_rgb(fg::white, color_ref);
            case 8:
                return bit4_to_rgb(fg::gray, color_ref);
            case 9:
                return bit4_to_rgb(fg::bright_red, color_ref);
            case 10:
                return bit4_to_rgb(fg::bright_green, color_ref);
            case 11:
                return bit4_to_rgb(fg::yellow, color_ref);
            case 12:
                return bit4_to_rgb(fg::bright_blue, color_ref);
            case 13:
                return bit4_to_rgb(fg::bright_magenta, color_ref);
            case 14:
                return bit4_to_rgb(fg::bright_cyan, color_ref);
            case 15:
                return bit4_to_rgb(fg::bright_white, color_ref);
            default:
                // not accessible, dummy error
                throw std::runtime_error(
                    "Term::bit8_to_rgb() failed with wrong value");
        }
    }
    // convert the actual 8bit color space
    else if(color <= 231) {
        // TODO
    }
    else {
        return {static_cast<uint8_t>(color / 24), static_cast<uint8_t>(color / 24), static_cast<uint8_t>(color / 24)};
    }
    // not accessible, dummy error
    throw std::runtime_error("conversion from 8bit to Term::RGB failed");
}

Term::RGB Term::bit8_to_rgb(uint8_t color) {
    return bit8_to_rgb(color, bit4_reference{});
}

Term::RGB Term::bit4_to_rgb(Term::fg color, Term::bit4_reference color_ref) {
    switch (color) {
        case fg::black:
            return color_ref.black;
        case fg::red:
            return color_ref.red;
        case fg::green:
            return color_ref.green;
        case fg::yellow:
            return color_ref.yellow;
        case fg::blue:
            return color_ref.blue;
        case fg::magenta:
            return color_ref.magenta;
        case fg::cyan:
            return color_ref.cyan;
        case fg::white:
            return color_ref.white;
        case fg::reset:
            // TODO
            break;
        case fg::gray:
            return color_ref.gray;
        case fg::bright_red:
            return color_ref.bright_red;
        case fg::bright_green:
            return color_ref.bright_green;
        case fg::bright_yellow:
            return color_ref.bright_yellow;
        case fg::bright_blue:
            return color_ref.bright_blue;
        case fg::bright_magenta:
            return color_ref.bright_magenta;
        case fg::bright_cyan:
            return color_ref.bright_cyan;
        case fg::bright_white:
            return color_ref.bright_white;
    }
    // not accessible
    return {};
}
Term::RGB Term::bit4_to_rgb(fg color) {
    return bit4_to_rgb(color, bit4_reference{});
}

Term::RGB Term::bit4_to_rgb(Term::bg color, Term::bit4_reference color_ref) {
    switch (color) {
        case bg::black:
            return color_ref.black;
        case bg::red:
            return color_ref.red;
        case bg::green:
            return color_ref.green;
        case bg::yellow:
            return color_ref.yellow;
        case bg::blue:
            return color_ref.blue;
        case bg::magenta:
            return color_ref.magenta;
        case bg::cyan:
            return color_ref.cyan;
        case bg::white:
            return color_ref.white;
        case bg::reset:
            // TODO
            break;
        case bg::gray:
            return color_ref.gray;
        case bg::bright_red:
            return color_ref.bright_red;
        case bg::bright_green:
            return color_ref.bright_green;
        case bg::bright_yellow:
            return color_ref.bright_yellow;
        case bg::bright_blue:
            return color_ref.bright_blue;
        case bg::bright_magenta:
            return color_ref.bright_magenta;
        case bg::bright_cyan:
            return color_ref.bright_cyan;
        case bg::bright_white:
            return color_ref.bright_white;
    }
    // no assessable
    return {};
}
Term::RGB Term::bit4_to_rgb(bg color) {
    return bit4_to_rgb(color, bit4_reference{});
}

uint16_t Term::rgb_diff(RGB color_first, RGB color_second) {
    uint16_t diff = 0;
    if (color_first.r > color_second.r) {
        diff += color_first.r - color_second.r;
    }
    else {
        diff += color_second.r - color_first.r;
    }
    if (color_first.g > color_second.g) {
        diff += color_first.g - color_second.g;
    }
    else {
        diff += color_second.g - color_first.g;
    }
    if (color_first.b > color_second.b) {
        diff += color_first.b - color_second.b;
    }
    else {
        diff += color_second.b - color_first.b;
    }
    return diff;
}

Term::bg Term::rgb_to_bit4_bg(Term::RGB color, Term::bit4_reference color_ref) {
    bg color_result = bg::black;

    uint16_t diff = rgb_diff(color, color_ref.black);
    if (diff > rgb_diff(color, color_ref.red)) {
        diff = rgb_diff(color, color_ref.red);
        color_result = bg::red;
    }
    if (diff > rgb_diff(color, color_ref.green)) {
        diff = rgb_diff(color, color_ref.green);
        color_result = bg::green;
    }
    if (diff > rgb_diff(color, color_ref.yellow)) {
        diff = rgb_diff(color, color_ref.yellow);
        color_result = bg::yellow;
    }
    if (diff > rgb_diff(color, color_ref.blue)) {
        diff = rgb_diff(color, color_ref.blue);
        color_result = bg::blue;
    }
    if (diff > rgb_diff(color, color_ref.magenta)) {
        diff = rgb_diff(color, color_ref.magenta);
        color_result = bg::magenta;
    }
    if (diff > rgb_diff(color, color_ref.cyan)) {
        diff = rgb_diff(color, color_ref.cyan);
        color_result = bg::cyan;
    }
    if (diff > rgb_diff(color, color_ref.white)) {
        diff = rgb_diff(color, color_ref.white);
        color_result = bg::white;
    }
    if (diff > rgb_diff(color, color_ref.gray)) {
        diff = rgb_diff(color, color_ref.gray);
        color_result = bg::gray;
    }
    if (diff > rgb_diff(color, color_ref.bright_red)) {
        diff = rgb_diff(color, color_ref.bright_red);
        color_result = bg::bright_red;
    }
    if (diff > rgb_diff(color, color_ref.bright_green)) {
        diff = rgb_diff(color, color_ref.bright_green);
        color_result = bg::bright_green;
    }
    if (diff > rgb_diff(color, color_ref.bright_yellow)) {
        diff = rgb_diff(color, color_ref.bright_yellow);
        color_result = bg::bright_yellow;
    }
    if (diff > rgb_diff(color, color_ref.bright_blue)) {
        diff = rgb_diff(color, color_ref.bright_blue);
        color_result = bg::bright_blue;
    }
    if (diff > rgb_diff(color, color_ref.bright_magenta)) {
        diff = rgb_diff(color, color_ref.bright_magenta);
        color_result = bg::bright_magenta;
    }
    if (diff > rgb_diff(color, color_ref.bright_cyan)) {
        diff = rgb_diff(color, color_ref.bright_cyan);
        color_result = bg::bright_cyan;
    }
    if (diff > rgb_diff(color, color_ref.bright_white)) {
        color_result = bg::bright_white;
    }
    return color_result;
}
Term::bg Term::rgb_to_bit4_bg(RGB color) {
    return rgb_to_bit4_bg(color, bit4_reference{});
}

Term::fg Term::rgb_to_bit4_fg(Term::RGB color, Term::bit4_reference color_ref) {
    fg color_result = fg::black;

    uint16_t diff = rgb_diff(color, color_ref.black);
    if (diff < rgb_diff(color, color_ref.red)) {
        diff = rgb_diff(color, color_ref.red);
        color_result = fg::red;
    }
    if (diff < rgb_diff(color, color_ref.green)) {
        diff = rgb_diff(color, color_ref.green);
        color_result = fg::green;
    }
    if (diff < rgb_diff(color, color_ref.yellow)) {
        diff = rgb_diff(color, color_ref.yellow);
        color_result = fg::yellow;
    }
    if (diff < rgb_diff(color, color_ref.blue)) {
        diff = rgb_diff(color, color_ref.blue);
        color_result = fg::blue;
    }
    if (diff < rgb_diff(color, color_ref.magenta)) {
        diff = rgb_diff(color, color_ref.magenta);
        color_result = fg::magenta;
    }
    if (diff < rgb_diff(color, color_ref.cyan)) {
        diff = rgb_diff(color, color_ref.cyan);
        color_result = fg::cyan;
    }
    if (diff < rgb_diff(color, color_ref.white)) {
        diff = rgb_diff(color, color_ref.white);
        color_result = fg::white;
    }
    if (diff < rgb_diff(color, color_ref.gray)) {
        diff = rgb_diff(color, color_ref.gray);
        color_result = fg::gray;
    }
    if (diff < rgb_diff(color, color_ref.bright_red)) {
        diff = rgb_diff(color, color_ref.bright_red);
        color_result = fg::bright_red;
    }
    if (diff < rgb_diff(color, color_ref.bright_green)) {
        diff = rgb_diff(color, color_ref.bright_green);
        color_result = fg::bright_green;
    }
    if (diff < rgb_diff(color, color_ref.bright_yellow)) {
        diff = rgb_diff(color, color_ref.bright_yellow);
        color_result = fg::bright_yellow;
    }
    if (diff < rgb_diff(color, color_ref.bright_blue)) {
        diff = rgb_diff(color, color_ref.bright_blue);
        color_result = fg::bright_blue;
    }
    if (diff < rgb_diff(color, color_ref.bright_magenta)) {
        diff = rgb_diff(color, color_ref.bright_magenta);
        color_result = fg::bright_magenta;
    }
    if (diff < rgb_diff(color, color_ref.bright_cyan)) {
        diff = rgb_diff(color, color_ref.bright_cyan);
        color_result = fg::bright_cyan;
    }
    if (diff < rgb_diff(color, color_ref.bright_white)) {
        color_result = fg::bright_white;
    }
    return color_result;
}
Term::fg Term::rgb_to_bit4_fg(RGB color) {
    return rgb_to_bit4_fg(color, bit4_reference{});
}

uint8_t Term::rgb_to_bit8(RGB color) {
    return 16 + (color.r * 6 / 256) * 36 + (color.g * 6 / 256) * 6 + (color.b * 6 / 256);
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
                         bool disable_ctrl_c,
                         bool _hide_cursor)
    : BaseTerminal(enable_keyboard, disable_ctrl_c),
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
