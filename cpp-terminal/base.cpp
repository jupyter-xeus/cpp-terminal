#include <cpp-terminal/base.hpp>
#include <cpp-terminal/private/platform.hpp>
#include <iostream>
#include <stdexcept>
#include "cpp-terminal/private/conversion.hpp"

/* COLOR CONVERSION */

Term::RGB Term::bit4_to_rgb(const Term::Color4 color) {
    switch (color) {
        case Color4::BLACK:
            return Bit4_reference::BLACK;
        case Color4::RED:
            return Bit4_reference::RED;
        case Color4::GREEN:
            return Bit4_reference::GREEN;
        case Color4::YELLOW:
            return Bit4_reference::YELLOW;
        case Color4::BLUE:
            return Bit4_reference::BLUE;
        case Color4::MAGENTA:
            return Bit4_reference::MAGENTA;
        case Color4::CYAN:
            return Bit4_reference::CYAN;
        case Color4::WHITE:
            return Bit4_reference::WHITE;
        case Color4::NONE:
            return Bit4_reference::NONE;
        case Color4::GRAY:
            return Bit4_reference::GRAY;
        case Color4::RED_BRIGHT:
            return Bit4_reference::RED_BRIGHT;
        case Color4::GREEN_BRIGHT:
            return Bit4_reference::GREEN_BRIGHT;
        case Color4::YELLOW_BRIGHT:
            return Bit4_reference::YELLOW_BRIGHT;
        case Color4::BLUE_BRIGHT:
            return Bit4_reference::BLUE_BRIGHT;
        case Color4::MAGENTA_BRIGHT:
            return Bit4_reference::MAGENTA_BRIGHT;
        case Color4::CYAN_BRIGHT:
            return Bit4_reference::CYAN_BRIGHT;
        case Color4::WHITE_BRIGHT:
            return Bit4_reference::WHITE_BRIGHT;
    }
    // impossible case
    return{};
}

Term::RGB Term::bit24_to_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return {r, g, b, false};
}
Term::RGB Term::rgb_empty() {
    return {0, 0, 0, true};
}

uint16_t Term::rgb_compare(RGB color_first, RGB color_second) {
    uint16_t diff = 0;
    // red
    if (color_first.r > color_second.r) {
        diff += color_first.r - color_second.r;
    } else {
        diff += color_second.r - color_first.r;
    }
    // green
    if (color_first.g > color_second.g) {
        diff += color_first.g - color_second.g;
    } else {
        diff += color_second.g - color_first.g;
    }
    // blue
    if (color_first.b > color_second.b) {
        diff += color_first.b - color_second.b;
    } else {
        diff += color_second.b - color_first.b;
    }

    return diff;
}

Term::Color4 Term::rgb_to_bit4(Term::RGB color) {
    // check if the RGB color is empty
    if (color.empty) {
        return Color4::NONE;
    }

    // set initial start value
    Color4 color_result = Color4::BLACK;
    uint16_t diff = rgb_compare(color, Term::Bit4_reference::BLACK);

    // compare all colors
    if (diff > rgb_compare(color, Bit4_reference::RED)) {
        diff = rgb_compare(color, Bit4_reference::RED);
        color_result = Color4::RED;
    }
    if (diff > rgb_compare(color, Bit4_reference::GREEN)) {
        diff = rgb_compare(color, Bit4_reference::GREEN);
        color_result = Color4::GREEN;
    }
    if (diff > rgb_compare(color, Bit4_reference::YELLOW)) {
        diff = rgb_compare(color, Bit4_reference::YELLOW);
        color_result = Color4::YELLOW;
    }
    if (diff > rgb_compare(color, Bit4_reference::BLUE)) {
        diff = rgb_compare(color, Bit4_reference::BLUE);
        color_result = Color4::BLUE;
    }
    if (diff > rgb_compare(color, Bit4_reference::MAGENTA)) {
        diff = rgb_compare(color, Bit4_reference::MAGENTA);
        color_result = Color4::MAGENTA;
    }
    if (diff > rgb_compare(color, Bit4_reference::CYAN)) {
        diff = rgb_compare(color, Bit4_reference::CYAN);
        color_result = Color4::CYAN;
    }
    if (diff > rgb_compare(color, Bit4_reference::WHITE)) {
        diff = rgb_compare(color, Bit4_reference::WHITE);
        color_result = Color4::WHITE;
    }
    if (diff > rgb_compare(color, Bit4_reference::GRAY)) {
        diff = rgb_compare(color, Bit4_reference::GRAY);
        color_result = Color4::GRAY;
    }
    if (diff > rgb_compare(color, Bit4_reference::RED_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::RED_BRIGHT);
        color_result = Color4::RED_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::GREEN_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::GREEN_BRIGHT);
        color_result = Color4::GREEN_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::YELLOW_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::YELLOW_BRIGHT);
        color_result = Color4::YELLOW_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::BLUE_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::BLUE_BRIGHT);
        color_result = Color4::BLUE_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::MAGENTA_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::MAGENTA_BRIGHT);
        color_result = Color4::MAGENTA_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::CYAN_BRIGHT)) {
        diff = rgb_compare(color, Bit4_reference::CYAN_BRIGHT);
        color_result = Color4::CYAN_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4_reference::WHITE_BRIGHT)) {
        color_result = Color4::WHITE_BRIGHT;
    }
    return color_result;
}

uint8_t Term::rgb_to_bit8(RGB color) {
    if (color.empty) {
        return 0;  // there is not really a way to handle empty color here
    }
    // check the color matches a 4bit color
    if (rgb_compare(color, Bit4_reference::BLACK) < 1) {
        return 0;
    } else if (rgb_compare(color, Bit4_reference::RED) < 1) {
        return 1;
    } else if (rgb_compare(color, Bit4_reference::GREEN) < 1) {
        return 2;
    } else if (rgb_compare(color, Bit4_reference::YELLOW) < 1) {
        return 3;
    } else if (rgb_compare(color, Bit4_reference::BLUE) < 1) {
        return 4;
    } else if (rgb_compare(color, Bit4_reference::MAGENTA) < 1) {
        return 5;
    } else if (rgb_compare(color, Bit4_reference::CYAN) < 1) {
        return 6;
    } else if (rgb_compare(color, Bit4_reference::WHITE) < 1) {
        return 7;
    } else if (rgb_compare(color, Bit4_reference::GRAY) < 1) {
        return 8;
    } else if (rgb_compare(color, Bit4_reference::RED_BRIGHT) < 1) {
        return 9;
    } else if (rgb_compare(color, Bit4_reference::GREEN_BRIGHT) < 1) {
        return 10;
    } else if (rgb_compare(color, Bit4_reference::YELLOW_BRIGHT) < 1) {
        return 11;
    } else if (rgb_compare(color, Bit4_reference::BLUE_BRIGHT) < 1) {
        return 12;
    } else if (rgb_compare(color, Bit4_reference::MAGENTA_BRIGHT) < 1) {
        return 13;
    } else if (rgb_compare(color, Bit4_reference::CYAN_BRIGHT) < 1) {
        return 14;
    } else if (rgb_compare(color, Bit4_reference::WHITE_BRIGHT) < 1) {
        return 15;
    }

    // check gray scale in 24 steps
    if (color.r == color.g && color.r == color.b && color.g == color.b) {
        return 232 + color.r / 32 + color.g / 32 + color.b / 32;
    }

    // normal color space
    return 16 + 36 * (color.r / 51) + 6 * (color.g / 51) + (color.b / 51);
}

bool Term::bit24_support() {
    std::string colorterm = getenv("COLORTERM");
    if (colorterm == "truecolor" || colorterm == "24bit") {
        return true;
    }
    return false;
}

std::string Term::rgb_to_bit24_auto_fg(RGB color) {
    if (bit24_support()) {
        return color_fg(color);
    } else {
        return color_fg(rgb_to_bit8(color));
    }
}

std::string Term::rgb_to_bit24_auto_bg(RGB color) {
    if (bit24_support()) {
        return color_bg(color);
    } else {
        return color_bg(rgb_to_bit8(color));
    }
}

/* FOREGROUND COLORS */

std::string Term::color_fg(Term::Color4 color) {
    return "\033[" + std::to_string((uint8_t)color + 30) + 'm';
}

std::string Term::color_fg(uint8_t color) {
    return "\033[38;5;" + std::to_string(color) + 'm';
}
std::string Term::color_fg(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[38;2;" + std::to_string(r) + ';' + std::to_string(g) + ';' +
           std::to_string(b) + 'm';
}

std::string Term::color_fg(Term::RGB rgb) {
    if (rgb.empty) {
        return color_fg(Color4::NONE);  // resets the current terminal color
    }
    return "\033[38;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) +
           ';' + std::to_string(rgb.b) + 'm';
}
std::string Term::color_fg(Term::RGBF rgbf) {
    return color_fg(rgbf, rgbf.mode_fg);
}

std::string Term::color_fg(Term::RGBF rgbf, Mode mode) {
    switch (mode) {
        case Mode::NONE:
            return color_fg(Color4::NONE);  // resets the current terminal color
        case Mode::BIT4:
            return color_fg(rgb_to_bit4(rgbf.rgb_fg));
        case Mode::BIT8:
            return color_fg(rgb_to_bit8(rgbf.rgb_fg));
        case Mode::BIT24:
            return color_fg(rgbf.rgb_fg);
        case Mode::AUTO24:
            return rgb_to_bit24_auto_fg(rgbf.rgb_fg);
    }
    // impossible case
    return{};
}

/* BACKGROUND COLORS */

std::string Term::color_bg(Term::Color4 color) {
    return "\033[" + std::to_string((uint8_t)color + 40) + 'm';
}
std::string Term::color_bg(uint8_t color) {
    return "\033[48;5;" + std::to_string(color) + 'm';
}
std::string Term::color_bg(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ';' + std::to_string(g) + ';' +
           std::to_string(b) + 'm';
}
std::string Term::color_bg(Term::RGB rgb) {
    if (rgb.empty) {
        return color_bg(Color4::NONE);  // resets the current terminal color
    }
    return "\033[48;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) +
           ';' + std::to_string(rgb.b) + 'm';
}
std::string Term::color_bg(Term::RGBF rgbf) {
    return color_bg(rgbf, rgbf.mode_bg);
}
std::string Term::color_bg(Term::RGBF rgbf, Mode mode) {
    switch (mode) {
        case Mode::NONE:
            return color_bg(Color4::NONE);  // resets the current terminal color
        case Mode::BIT4:
            return color_bg(rgb_to_bit4(rgbf.rgb_bg));
        case Mode::BIT8:
            return color_bg(rgb_to_bit8(rgbf.rgb_bg));
        case Mode::BIT24:
            return color_bg(rgbf.rgb_bg);
        case Mode::AUTO24:
            return rgb_to_bit24_auto_bg(rgbf.rgb_bg);
    }
    // impossible case
    return{};
}
std::string Term::style(Term::Style style) {
    return "\033[" + std::to_string((uint8_t)style) + 'm';
}

/* RGBF FUNCTIONS */

std::string Term::colorf(Term::RGBF rgbf) {
    return color_fg(rgbf) + color_bg(rgbf);
}

Term::RGBF Term::rgbf_fg(Term::Color4 color) {
    return {bit4_to_rgb(color), Mode::BIT4, rgb_empty(), Mode::NONE};
}
Term::RGBF Term::rgbf_fg(Term::RGB rgb) {
    return {rgb, Mode::BIT24, rgb_empty(), Mode::NONE};
}

Term::RGBF Term::rgbf_fg(uint8_t r, uint8_t g, uint8_t b) {
    return {{r, g, b}, Mode::BIT24, rgb_empty(), Mode::NONE};
}

Term::RGBF Term::rgbf_bg(Term::Color4 color) {
    return {rgb_empty(), Mode::NONE, bit4_to_rgb(color), Mode::BIT4};
}
Term::RGBF Term::rgbf_bg(Term::RGB rgb) {
    return {rgb_empty(), Mode::NONE, rgb, Mode::BIT24};
}
Term::RGBF Term::rgbf_bg(uint8_t r, uint8_t g, uint8_t b) {
    return {rgb_empty(), Mode::NONE, {r, g, b}, Mode::BIT24};
}

Term::RGBF Term::rgbf_fb(Term::Color4 fg, Term::Color4 bg) {
    return {bit4_to_rgb(fg), Mode::BIT4, bit4_to_rgb(bg), Mode::BIT4};
}

Term::RGBF Term::rgbf_fb(Term::RGB rgb_fg, Term::RGB rgb_bg) {
    return {rgb_fg, Mode::BIT24, rgb_bg, Mode::BIT24};
}
Term::RGBF Term::rgbf_fb(uint8_t r_fg,
                         uint8_t g_fg,
                         uint8_t b_fg,
                         uint8_t r_bg,
                         uint8_t g_bg,
                         uint8_t b_bg) {
    return {{r_fg, g_fg, b_fg}, Mode::BIT24, {r_bg, g_bg, b_bg}, Mode::BIT24};
}

Term::RGBF Term::rgbf_empty() {
    return {rgb_empty(), Mode::NONE, rgb_empty(), Mode::NONE};
}

/* AUTOMATIC COLORS */

std::string Term::color_auto(Term::RGBF rgbf, Term::Mode mode) {
    return color_fg(rgbf, mode) + color_bg(rgbf, mode);
}

std::string Term::color_auto(Term::RGBF rgbf) {
    return color_fg(rgbf) + color_bg(rgbf);
}

std::tuple<size_t, size_t> Term::get_size() {
    return Private::get_term_size();  // function uses platform dependent code
}

bool Term::stdin_connected() {
    return Private::is_stdin_a_tty();
}

bool Term::stdout_connected() {
    return Private::is_stdout_a_tty();
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
std::string Term::clear_buffer() {
    return "\033[3J";
}

std::string Term::cursor_move(size_t row, size_t column) {
    return "\033[" + std::to_string(row) + ';' + std::to_string(column) + 'H';
}

std::string Term::cursor_up(size_t rows) {
    return "\033[" + std::to_string(rows) + 'A';
}

std::string Term::cursor_down(size_t rows) {
    return "\033[" + std::to_string(rows) + 'B';
}

std::string Term::cursor_right(size_t columns) {
    return "\033[" + std::to_string(columns) + 'C';
}

std::string Term::cursor_left(size_t columns) {
    return "\033[" + std::to_string(columns) + 'D';
}

std::tuple<size_t, size_t> Term::cursor_position() {
    char buf[32];
    // write cursor position report
    std::cout << cursor_position_report() << std::flush;
    // read input buffer
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
        size_t rows, columns;
        if (buf[i] == '\x1b' && buf[i + 1] == '[') {
            if (Private::unified_sscanf(&buf[i + 2], "%d;%d", &rows,
                                        &columns) != 2) {
                throw std::runtime_error(
                    "get_cursor_position(): result could not be parsed");
            }
            return {rows, columns};
        }
        if (buf[i] == '\0')
            break;
    }
    throw std::runtime_error(
        "get_cursor_position(): result not found in the response");
}

std::string Term::cursor_position_report() {
    return "\x1b[6n";
}

std::string Term::clear_eol() {
    return "\033[K";
}

std::string Term::screen_save() {
    return "\0337"         // save current cursor position
           "\033[?1049h";  // save screen
}
std::string Term::screen_load() {
    return "\033[?1049l"  // restores screen
           "\0338";       // restore current cursor position
}
std::string Term::terminal_title(const std::string& title) {
    return "\033]0;" + title + '\a';
}

Term::Terminal::Terminal(bool _clear_screen,
                         bool enable_keyboard,
                         bool disable_signal_keys,
                         bool _hide_cursor)
    : BaseTerminal(enable_keyboard, disable_signal_keys),
      clear_screen{_clear_screen},
      hide_cursor{_hide_cursor} {
    if (clear_screen) {
        std::cout << screen_save() +
                         // Fix consoles that ignore save_screen()
                         clear_buffer();
    }
    if (hide_cursor)
        std::cout << cursor_off();

    // flush stdout
    std::cout << std::flush;
}
Term::Terminal::Terminal(bool _clear_screen)
    : BaseTerminal(false, true), clear_screen{_clear_screen} {
    if (clear_screen) {
        std::cout << screen_save() << clear_buffer() << std::flush;
    }
}
Term::Terminal::~Terminal() {
    if (clear_screen) {
        // Fix consoles that ignore save_screen()
        std::cout << style(Style::RESET) << clear_buffer() << cursor_move(1, 1)
                  << screen_load();
    }
    if (hide_cursor)
        std::cout << cursor_on();

    // flush the output stream
    std::cout << std::flush;
}
