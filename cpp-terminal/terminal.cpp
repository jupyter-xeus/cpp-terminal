#include <cpp-terminal/terminal.h>
#include <cpp-terminal/terminal_base.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cpp-terminal/base.hpp>

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

int Term::Terminal::read_key() const {
    int key{};
    while ((key = read_key0()) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return key;
}

int Term::Terminal::read_key0() const {
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

void Term::Terminal::get_cursor_position(int& rows, int& cols) const {
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
