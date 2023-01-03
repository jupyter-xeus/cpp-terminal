#pragma once

#include <string>
#include <tuple>

namespace Term
{
namespace Private
{
// Returns true if the standard input is attached to a terminal
bool        is_stdin_a_tty();
// Returns true if the standard output is attached to a terminal
bool        is_stdout_a_tty();
// Get the environment variable
std::string getenv(const std::string&);

// returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
// if the console is not connected
std::tuple<std::size_t, std::size_t> get_term_size();

// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
bool read_raw(char* s);

bool has_ansi_escape_code();

char read_raw_stdin();

}  // namespace Private
}  // namespace Term
