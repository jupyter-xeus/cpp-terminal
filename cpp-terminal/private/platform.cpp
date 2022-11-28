#include "platform.hpp"

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#endif

#include <tuple>

#include <stdexcept>

void Term::Private::BaseTerminal::store_and_restore() {
    static bool enabled{false};
#ifdef _WIN32
    static DWORD dwOriginalOutMode{};
    static UINT out_code_page{GetConsoleOutputCP()};
    static DWORD dwOriginalInMode{};
    static UINT in_code_page{GetConsoleCP()};
    if (!enabled) {
        if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),
                            &dwOriginalOutMode)) {
            throw std::runtime_error("GetConsoleMode() failed");
        }
        if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),
                            &dwOriginalInMode)) {
            throw std::runtime_error("GetConsoleMode() failed");
        }
        enabled = true;
    } else {
        // Should be safe to restore previous value all the time;
        SetConsoleOutputCP(out_code_page);
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),
                            dwOriginalOutMode)) {
            throw std::runtime_error("SetConsoleMode() failed in destructor");
        }
        SetConsoleCP(in_code_page);
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwOriginalInMode)) {
            throw std::runtime_error("SetConsoleMode() failed in destructor");
        }
    }
#else
    static termios orig_termios{};
    if (!enabled) {
        if (keyboard_enabled) {
            if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
                throw std::runtime_error("tcgetattr() failed");
            }
        }
        enabled = true;
    } else {
        if (keyboard_enabled) {
            if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
                throw std::runtime_error("tcsetattr() failed in destructor");
            }
        }
    }
#endif
}

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

std::string Term::Private::getenv(const std::string& env) {
#if _WIN32
    std::size_t requiredSize{0};
    getenv_s(&requiredSize, nullptr, 0, env.c_str());
    if (requiredSize == 0)
        return std::string();
    std::string ret;
    ret.reserve(requiredSize * sizeof(char));
    getenv_s(&requiredSize, &ret[0], requiredSize, env.c_str());
#else
    if (std::getenv(env.c_str()) != nullptr)
        return static_cast<std::string>(std::getenv(env.c_str()));
    else
        return std::string();
#endif
}

std::tuple<std::size_t, std::size_t> Term::Private::get_term_size() {
#ifdef _WIN32
    if (GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed");
    }
    CONSOLE_SCREEN_BUFFER_INFO inf;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf)) {
        std::size_t cols = inf.srWindow.Right - inf.srWindow.Left + 1;
        std::size_t rows = inf.srWindow.Bottom - inf.srWindow.Top + 1;
        return {rows, cols};
    } else {
        // This happens when we are not connected to a terminal
        throw std::runtime_error(
            "Couldn't get terminal size. Is it connected to a TTY?");
    }
#else
    struct winsize ws {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // This happens when we are not connected to a terminal
        throw std::runtime_error(
            "Couldn't get terminal size. Is it connected to a TTY?");
    } else {
        return std::tuple<std::size_t, std::size_t>{ws.ws_row, ws.ws_col};
    }
#endif
}
char Term::Private::read_raw_stdin() {
    int c = getchar();
    if (c >= 0) {
        return c;
    } else if (c == EOF) {
        // In non-raw (blocking) mode this happens when the input file
        // ends. In such a case, return the End of Transmission (EOT)
        // character (Ctrl-D)
        return 0x04;
    } else {
        throw std::runtime_error("getchar() failed");
    }
}

bool Term::Private::read_raw(char* s) {
    // do nothing when TTY is not connected
    if (!is_stdin_a_tty()) {
        return false;
    }
#ifdef _WIN32
    if (GetStdHandle(STD_INPUT_HANDLE) == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed");
    }
    char buf[1];
    DWORD nread;
    if (_kbhit()) {
        if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), buf, 1, &nread,
                      nullptr)) {
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

bool Term::Private::has_ansi_escape_code() {
#ifdef _WIN32
    static bool checked{false};
    static bool has_ansi{false};
    if (checked == false) {
        const DWORD MINV_MAJOR{10};
        const DWORD MINV_MINOR{0};
        const DWORD MINV_BUILD{10586};
        HMODULE hMod{GetModuleHandle(TEXT("ntdll.dll"))};
        if (hMod) {
            RtlGetVersionPtr fn = {reinterpret_cast<RtlGetVersionPtr>(
                GetProcAddress(hMod, "RtlGetVersion"))};
            if (fn != nullptr) {
                RTL_OSVERSIONINFOW rovi{0};
                rovi.dwOSVersionInfoSize = sizeof(rovi);
                if (fn(&rovi) == 0) {
                    if (rovi.dwMajorVersion > MINV_MAJOR ||
                        (rovi.dwMajorVersion == MINV_MAJOR &&
                         (rovi.dwMinorVersion > MINV_MINOR ||
                          (rovi.dwMinorVersion == MINV_MINOR &&
                           rovi.dwBuildNumber >= MINV_BUILD)))) {
                        has_ansi = true;
                    }
                }
            }
        }
        checked = true;
    }
    return has_ansi;
#else
    return true;
#endif
}

Term::Private::BaseTerminal::~BaseTerminal() noexcept(false) {
    store_and_restore();
}

Term::Private::BaseTerminal::BaseTerminal(bool enable_keyboard,
                                          bool disable_signal_keys)
    : keyboard_enabled{enable_keyboard} {
    store_and_restore();
#ifdef _WIN32
    // silently disable raw mode for non-tty
    if (keyboard_enabled)
        keyboard_enabled = is_stdin_a_tty();
    if (is_stdout_a_tty()) {
        SetConsoleOutputCP(65001);
        if (GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed");
        }
        DWORD flags{0};
        if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &flags)) {
            throw std::runtime_error("GetConsoleMode() failed");
        }
        if (has_ansi_escape_code()) {
            flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            flags |= DISABLE_NEWLINE_AUTO_RETURN;
        }
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), flags)) {
            throw std::runtime_error("SetConsoleMode() failed");
        }
    }

    if (keyboard_enabled) {
        SetConsoleCP(65001);
        if (GetStdHandle(STD_INPUT_HANDLE) == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed");
        }
        DWORD flags{0};
        if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &flags)) {
            throw std::runtime_error("GetConsoleMode() failed");
        }
        if (has_ansi_escape_code()) {
            flags |= ENABLE_VIRTUAL_TERMINAL_INPUT;
        }
        if (disable_signal_keys) {
            flags &= ~ENABLE_PROCESSED_INPUT;
        }
        flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), flags)) {
            throw std::runtime_error("SetConsoleMode() failed");
        }
    }
#else
    // silently disable raw mode for non-tty
    if (keyboard_enabled)
        keyboard_enabled = is_stdin_a_tty();
    termios raw{};
    if (keyboard_enabled) {
        if (tcgetattr(STDIN_FILENO, &raw) == -1) {
            throw std::runtime_error("tcgetattr() failed");
        }

        // Put terminal in raw mode

        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

        // This disables output post-processing, requiring explicit \r\n. We
        // keep it enabled, so that in C++, one can still just use std::endl
        // for EOL instead of "\r\n".
        // raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
        if (disable_signal_keys) {
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
