#pragma once

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

namespace Term::Private {
// Returns true if the standard input is attached to a terminal
bool is_stdin_a_tty();
// Returns true if the standard output is attached to a terminal
bool is_stdout_a_tty();

bool get_term_size(int& rows, int& cols);

// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
bool read_raw(char* s);

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

    virtual ~BaseTerminal() noexcept(false);
};

}  // namespace Term::Private
