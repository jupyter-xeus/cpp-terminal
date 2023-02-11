#pragma once

#include <string>
#include <utility>

namespace Term
{
namespace Private
{

// Get the environment variable
std::string getenv(const std::string&);

// returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
// if the console is not connected
std::pair<std::size_t, std::size_t> get_term_size();

// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
bool read_raw(char* s);


char read_raw_stdin();

}  // namespace Private
}  // namespace Term
