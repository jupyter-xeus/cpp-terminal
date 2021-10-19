#include <cpp-terminal/terminal_base.h>

namespace Term {

#ifdef _WIN32
    BaseTerminal::BaseTerminal(bool enable_keyboard,
                          bool /*disable_ctrl_c*/)
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
    BaseTerminal::BaseTerminal(bool enable_keyboard,
                          bool disable_ctrl_c)
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

}  // namespace Term
