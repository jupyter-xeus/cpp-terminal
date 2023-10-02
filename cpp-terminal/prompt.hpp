/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/window.hpp"

#include <functional>

namespace Term
{
/* Basic prompt */

// indicates the results of prompt_blocking() and prompt_non_blocking
enum class Result
{
  // returned if the user chose yes
  YES,
  // returned if the user chose no
  NO,
  // returned of no terminal is attached to the program
  ERROR,
  // returned of the enter key was pressed without additional input
  NONE,
  // returned if CTRL+C was pressed
  ABORT,
  // returned if the given input did not match the case 'yes' of 'no'
  INVALID
};
// indicates the results of prompt_simple()
enum class Result_simple
{
  // returned if the user chose yes
  YES,
  // returned if the user chose no or invalid / no input or if no terminal is
  // attached
  NO,
  // returned if CTRL+C was pressed
  ABORT
};

// A simple yes/no prompt, requires the user to press the ENTER key to continue
// The arguments are used like this: 1 [2/3]4 <user Input>
// the immediate switch indicates toggles whether pressing enter for
// confirming the input is required or not
Result prompt(const std::string& message, const std::string& first_option, const std::string& second_option, const std::string& prompt_indicator, bool);

// The most simple prompt possible, requires the user to press enter to continue
// The arguments are used like this: 1 [y/N]:
// Invalid input, errors (like no attached terminal) all result in 'no' as
// default
Result_simple prompt_simple(const std::string& message);

/* Multiline prompt */

// This model contains all the information about the state of the prompt in an
// abstract way, irrespective of where or how it is rendered.
class Model
{
public:
  std::string              prompt_string;  // The string to show as the prompt
  std::vector<std::string> lines{""};      // The current input string in the prompt as a vector of lines, without '\n' at the end.
  // The current cursor position in the "input" string, starting from (1,1)
  std::size_t              cursor_col{1};
  std::size_t              cursor_row{1};
};

std::string concat(const std::vector<std::string>&);

std::vector<std::string> split(const std::string&);

void print_left_curly_bracket(Term::Window&, const std::size_t&, const std::size_t&, const std::size_t&);

void render(Term::Window&, const Model&, const std::size_t&);

std::string prompt_multiline(const std::string&, std::vector<std::string>&, std::function<bool(std::string)>&);
}  // namespace Term
