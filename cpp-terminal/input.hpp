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
}  // namespace Platform

// Waits for an event (key press, screen resizing ...)
Term::Event read_event();

}  // namespace Term
