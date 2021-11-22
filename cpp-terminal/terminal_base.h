#ifndef TERMINAL_BASE_H
#define TERMINAL_BASE_H

/*
 * This file contains all the platform specific code regarding terminal input
 * and output. The rest of the code does not have any platform specific
 * details. This file is designed in a way to contain the least number of
 * building blocks, that the rest of the code can use to build all the
 * features.
 */

#include <stdexcept>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#undef B0
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B28800
#undef B38400
#undef B57600
#undef B115200
#include <unistd.h>
#include <cerrno>
#endif

namespace Term {

/* Note: the code that uses Terminal must be inside try/catch block, otherwise
 * the destructors will not be called when an exception happens and the
 * terminal will not be left in a good state. Terminal uses exceptions when
 * something goes wrong.
 */
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
    explicit BaseTerminal(bool enable_keyboard = false,
                          bool disable_ctrl_c = true);

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

}  // namespace Term

#endif  // TERMINAL_BASE_H
