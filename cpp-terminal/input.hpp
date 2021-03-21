#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <termios.h>
#endif

#include <thread>
#include <chrono>
#include <cpp-terminal/base.hpp>
#include <cpp-terminal/tools.hpp>

namespace Term {
enum Key {
    BACKSPACE = 1000,
    ENTER,
    ALT_ENTER,
    TAB,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_UP,
    CTRL_DOWN,
    CTRL_RIGHT,
    CTRL_LEFT,
    NUMERIC_5,
    DEL,
    HOME,
    INSERT,
    END,
    PAGE_UP,
    PAGE_DOWN,
    ESC,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    // special keys
    CTRL = -96,
    ALT = -128
};


// TODO merge BaseTerminal and Terminal together
class BaseTerminal {
   private:
#ifdef _WIN32
    HANDLE hout;
    DWORD dwOriginalOutMode{};
    bool out_console;
    UINT out_code_page;

    HANDLE hin;
    DWORD dwOriginalInMode{};
    UINT in_code_page;
#else
    struct termios orig_termios {};
#endif
    bool keyboard_enabled{};

   public:
#ifdef _WIN32
    explicit BaseTerminal(bool enable_keyboard = false,
                          bool /*disable_ctrl_c*/ = true)
        : keyboard_enabled{enable_keyboard} {
        // Uncomment this to silently disable raw mode for non-tty
        // if (keyboard_enabled) keyboard_enabled = is_stdin_a_tty();
        out_console = is_stdout_a_tty();
        if (out_console) {
            hout = GetStdHandle(STD_OUTPUT_HANDLE);
            out_code_page = GetConsoleOutputCP();
            SetConsoleOutputCP(65001);
            if (hout == INVALID_HANDLE_VALUE) {
                throw std::runtime_error(
                    "GetStdHandle(STD_OUTPUT_HANDLE) failed");
            }
            if (!GetConsoleMode(hout, &dwOriginalOutMode)) {
                throw std::runtime_error("GetConsoleMode() failed");
            }
            DWORD flags = dwOriginalOutMode;
            flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            flags |= DISABLE_NEWLINE_AUTO_RETURN;
            if (!SetConsoleMode(hout, flags)) {
                throw std::runtime_error("SetConsoleMode() failed");
            }
        }

        if (keyboard_enabled) {
            hin = GetStdHandle(STD_INPUT_HANDLE);
            in_code_page = GetConsoleCP();
            SetConsoleCP(65001);
            if (hin == INVALID_HANDLE_VALUE) {
                throw std::runtime_error(
                    "GetStdHandle(STD_INPUT_HANDLE) failed");
            }
            if (!GetConsoleMode(hin, &dwOriginalInMode)) {
                throw std::runtime_error("GetConsoleMode() failed");
            }
            DWORD flags = dwOriginalInMode;
            flags |= ENABLE_VIRTUAL_TERMINAL_INPUT;
            flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
            if (!SetConsoleMode(hin, flags)) {
                throw std::runtime_error("SetConsoleMode() failed");
            }
        }
#else
    explicit BaseTerminal(bool enable_keyboard = false,
                          bool disable_ctrl_c = true)
        : keyboard_enabled{enable_keyboard} {
        // Uncomment this to silently disable raw mode for non-tty
        // if (keyboard_enabled) keyboard_enabled = is_stdin_a_tty();
        if (keyboard_enabled) {
            if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
                throw std::runtime_error("tcgetattr() failed");
            }

            // Put terminal in raw mode
            struct termios raw = orig_termios;
            raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
            // This disables output post-processing, requiring explicit \r\n. We
            // keep it enabled, so that in C++, one can still just use std::endl
            // for EOL instead of "\r\n".
            // raw.c_oflag &= ~(OPOST);
            raw.c_cflag |= (CS8);
            raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
            if (disable_ctrl_c) {
                raw.c_lflag &= ~(ISIG);
            }
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;

            if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
                throw std::runtime_error("tcsetattr() failed");
            }
        }
#endif
    }

    virtual ~BaseTerminal() noexcept(false) {
#ifdef _WIN32
        if (out_console) {
            SetConsoleOutputCP(out_code_page);
            if (!SetConsoleMode(hout, dwOriginalOutMode)) {
                throw std::runtime_error(
                    "SetConsoleMode() failed in destructor");
            }
        }

        if (keyboard_enabled) {
            SetConsoleCP(in_code_page);
            if (!SetConsoleMode(hin, dwOriginalInMode)) {
                throw std::runtime_error(
                    "SetConsoleMode() failed in destructor");
            }
        }
#else
        if (keyboard_enabled) {
            if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
                throw std::runtime_error("tcsetattr() failed in destructor");
            }
        }
#endif
    }

    // Returns true if a character is read, otherwise immediately returns false
    bool read_raw(char* s) const {
        if (!keyboard_enabled) {
            int c = getchar();
            if (c >= 0) {
                *s = c;
            } else if (c == EOF) {
                // In non-raw (blocking) mode this happens when the input file
                // ends. In such a case, return the End of Transmission (EOT)
                // character (Ctrl-D)
                *s = 0x04;
            } else {
                throw std::runtime_error("getchar() failed");
            }
            return true;
        }
#ifdef _WIN32
        char buf[1];
        DWORD nread;
        if (_kbhit()) {
            if (!ReadFile(hin, buf, 1, &nread, nullptr)) {
                throw std::runtime_error("ReadFile() failed");
            }
            if (nread == 1) {
                *s = buf[0];
                return true;
            } else {
                throw std::runtime_error("kbhit() and ReadFile() inconsistent");
            }
        } else {
            return false;
        }
#else
        int nread = read(STDIN_FILENO, s, 1);
        if (nread == -1 && errno != EAGAIN) {
            throw std::runtime_error("read() failed");
        }
        return (nread == 1);
#endif
    }
};


class Terminal : public BaseTerminal {
    bool restore_screen_;

   public:
    explicit Terminal(bool enable_keyboard = false, bool disable_ctrl_c = true)
        : BaseTerminal(enable_keyboard, disable_ctrl_c),
          restore_screen_{false} {}

    ~Terminal() override { restore_screen(); }

    void restore_screen() {
        if (restore_screen_) {
            Term::restore_screen();
        }
    }

    void save_screen() {
        restore_screen_ = true;
        Term::save_screen();
    }

    // Waits for a key press, translates escape codes
    int read_key() const {
        int key{};
        while ((key = read_key0()) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return key;
    }

    // If there was a key press, returns the translated key from escape codes,
    // otherwise returns 0. If the escape code is not supported, returns a
    // negative number.
    int read_key0() const {
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

    void get_cursor_position(int& rows, int& cols) const {
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

    // This function takes about 23ms, so it should only be used as a fallback
    void get_term_size_slow(int& rows, int& cols) const {
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
};


}  // namespace Term
