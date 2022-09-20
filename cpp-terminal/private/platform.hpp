#pragma once

#ifdef _WIN32
#define NOMINMAX
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || \
    defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(i386) || defined(__i386) || defined(__i386__) || \
    defined(__i386__) || defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
#ifndef _ARM_
#define _ARM_
#endif
#endif
#include <minwindef.h>
#undef NOMINMAX
#include <tuple>
#else
class termios;
#include <memory>
#endif

#include <string>

namespace Term::Private {
// Returns true if the standard input is attached to a terminal
bool is_stdin_a_tty();
// Returns true if the standard output is attached to a terminal
bool is_stdout_a_tty();
// Get the environment variable
std::string getenv(const std::string&);

// returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
// if the console is not connected
std::tuple<std::size_t, std::size_t> get_term_size();

// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
bool read_raw(char* s);

bool has_ansi_escape_code();

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
    std::unique_ptr<termios> orig_termios{nullptr};
#endif
    bool keyboard_enabled{};

   public:
    explicit BaseTerminal(bool enable_keyboard = false,
                          bool disable_signal_keys = true);

    virtual ~BaseTerminal() noexcept(false);
};

char read_raw_stdin();

}  // namespace Term::Private
