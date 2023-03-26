#pragma once

#include "cpp-terminal/event.hpp"

#include <string>

namespace Term
{

namespace Platform
{
// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
Term::Event read_raw();

char read_raw_stdin();
}  // namespace Platform

// Waits for an event (key press, screen resizing ...)
Term::Event read_event();

// returns the stdin as a string
// waits until the EOT signal is send
// if Term::Terminal is not enabling the keyboard this function will wait until
// the user presses CTRL+D (which sends the EOT signal)
std::string read_stdin();

}  // namespace Term
