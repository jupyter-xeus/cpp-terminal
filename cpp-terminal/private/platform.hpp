#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace Term::Private {
// Returns true if the standard input is attached to a terminal
inline bool is_stdin_a_tty() {
#ifdef _WIN32
    return _isatty(_fileno(stdin));
#else
    return isatty(STDIN_FILENO);
#endif
}
// Returns true if the standard output is attached to a terminal
inline bool is_stdout_a_tty() {
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

    inline bool get_term_size(int& rows, int& cols) {
#ifdef _WIN32
        HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hout == INVALID_HANDLE_VALUE) {
            throw std::runtime_error(
                "GetStdHandle(STD_OUTPUT_HANDLE) failed");
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

}  // namespace Term::Platform
