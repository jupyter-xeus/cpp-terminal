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


    // Returns true if a character is read, otherwise immediately returns false
    // This can't be made inline
    static bool read_raw(char* s) {
        // TODO: What if the keyboard is not initialzed?
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

}  // namespace Term::Platform
