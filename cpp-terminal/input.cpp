#include <chrono>
#include <cpp-terminal/base.hpp>
#include <cpp-terminal/input.hpp>
#include <iostream>
#include <thread>
#include "private/platform.hpp"

bool Term::is_ANSII(const Term::Key& key) {
    if (key >= 0 && key <= 127)
        return true;
    else
        return false;
}

bool Term::is_extended_ANSII(const Term::Key& key) {
    if (key >= 0 && key <= 255)
        return true;
    else
        return false;
}

bool Term::is_CTRL(const Term::Key& key) {
    // Need to supress the TAB etc...
    if (key > 0 && key <= 31 && key != Key::BACKSPACE && key != Key::TAB &&
        key != ESC && /* the two mapped to ENTER */ key != Key::LF && key != CR)
        return true;
    else
        return false;
}

bool Term::is_ALT(const Term::Key& key) {
    if ((key & Key::ALT) == Key::ALT)
        return true;
    else
        return false;
}

std::int32_t Term::read_key() {
    std::int32_t key{Key::NO_KEY};
    while ((key = read_key0()) == Key::NO_KEY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return key;
}

std::int32_t Term::read_key0() {
    char c{'\0'};
    if (!Private::read_raw(&c))
        return Key::NO_KEY;
    if (is_CTRL(static_cast<Term::Key>(c))) {
        return c;
    } else if (c == Key::ESC) {
        char seq[4]{'\0', '\0', '\0', '\0'};

        if (!Private::read_raw(&seq[0]))
            return Key::ESC;
        if (!Private::read_raw(&seq[1])) {
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
                if (!Private::read_raw(&seq[2])) {
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
                        if (!Private::read_raw(&seq[2])) {
                            return -10;
                        }
                        if (!Private::read_raw(&seq[3])) {
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
                        if (!Private::read_raw(&seq[3])) {
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
            case Key::LF:
            case Key::CR:
                return Key::ENTER;
        }
        if (c == '\xc3') {
            if (!Private::read_raw(&c)) {
                return -8;
            } else {
                if (c >= '\xa1' && c <= '\xba') {
                    // xterm
                    return Key::ALT + (c + 'a' - '\xa1');
                }
                return -9;
            }
        } else if (c == '\xc2') {
            if (!Private::read_raw(&c)) {
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

// returns the whole input from STDIN as string
std::string Term::read_stdin() {
    std::string file;
    char c;
    while (true) {
        c = Private::read_raw_stdin();
        if (c == 0x04) {  // check for end of transmission signal
            return file;
        } else {
            file += c;
        }
    }
}
std::string Term::read_stdin_alone() {
    // temporarily enable raw mode
    Term::Terminal term(false, true, false, false);
    return Term::read_stdin();
}
