#include "platform.hpp"

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#endif

#include <stdexcept>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

bool Term::Private::is_stdin_a_tty() {
#ifdef _WIN32
    return _isatty(_fileno(stdin));
#else
    return isatty(STDIN_FILENO);
#endif
}

bool Term::Private::is_stdout_a_tty() {
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

bool Term::Private::get_term_size(int& rows, int& cols) {
#ifdef _WIN32
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hout == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed");
    }
    CONSOLE_SCREEN_BUFFER_INFO inf;
    if (GetConsoleScreenBufferInfo(hout, &inf)) {
        cols = inf.srWindow.Right - inf.srWindow.Left + 1;
        rows = inf.srWindow.Bottom - inf.srWindow.Top + 1;
        return true;
    } else {
        // This happens when we are not connected to a terminal
        return false;
    }
#else
    struct winsize ws {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // This happens when we are not connected to a terminal
        return false;
    } else {
        cols = ws.ws_col;
        rows = ws.ws_row;
        return true;
    }
#endif
}

bool Term::Private::read_raw(char* s) {
    // TODO: What if the keyboard is not initialized?
    if (false) {
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
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    if (hin == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed");
    }
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
    ssize_t nread = read(STDIN_FILENO, s, 1);
    if (nread == -1 && errno != EAGAIN) {
        throw std::runtime_error("read() failed");
    }
    return (nread == 1);
#endif
}

Term::Private::BaseTerminal::~BaseTerminal() noexcept(false) {
#ifdef _WIN32
    if (out_console) {
        SetConsoleOutputCP(out_code_page);
        if (!SetConsoleMode(hout, dwOriginalOutMode)) {
            throw std::runtime_error("SetConsoleMode() failed in destructor");
        }
    }

    if (keyboard_enabled) {
        SetConsoleCP(in_code_page);
        if (!SetConsoleMode(hin, dwOriginalInMode)) {
            throw std::runtime_error("SetConsoleMode() failed in destructor");
        }
    }
#else
    if (keyboard_enabled) {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios.get()) == -1) {
            throw std::runtime_error("tcsetattr() failed in destructor");
        }
    }
    // Just to let it living longer
    orig_termios.get();
#endif
}

#ifdef _WIN32
Term::Private::BaseTerminal::BaseTerminal(bool enable_keyboard,
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
            throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed");
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
            throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed");
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
Term::Private::BaseTerminal::BaseTerminal(bool enable_keyboard,
                                          bool disable_ctrl_c)
    : orig_termios{std::unique_ptr<termios>(new termios)},
      keyboard_enabled{enable_keyboard} {
    // Uncomment this to silently disable raw mode for non-tty
    // if (keyboard_enabled) keyboard_enabled = is_stdin_a_tty();
    if (keyboard_enabled) {
        if (tcgetattr(STDIN_FILENO, orig_termios.get()) == -1) {
            throw std::runtime_error("tcgetattr() failed");
        }

        // Put terminal in raw mode

        struct termios raw = termios(*orig_termios.get());

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
