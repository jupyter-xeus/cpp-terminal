/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/terminal_impl.hpp"
#include "cpp-terminal/terminal_initializer.hpp"

#include <string>

namespace Term
{

static const TerminalInitializer terminal_initializer;  //NOLINT(cert-err58-cpp,fuchsia-statically-constructed-objects)
extern Term::Terminal&           terminal;

// change the title of the terminal, only supported by a few terminals
std::string terminal_title(const std::string& title);
// clear the screen and the scroll-back buffer
std::string clear_buffer();

}  // namespace Term
