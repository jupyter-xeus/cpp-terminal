/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/window.hpp"

#include <functional>

namespace Term
{

// indicates the results of prompt_blocking() and prompt_non_blocking
enum class Result
{
  Yes,     ///< Returned if the user chose \b yes.
  No,      ///< Returned if the user chose \b no.
  Error,   ///< Returned if no terminal is attached to the program.
  None,    ///< Returned if the enter key was pressed without additional input.
  Abort,   ///< Returned if CTRL+C was pressed.
  Invalid  ///< Returned if the given input did not match the case \b yes of \b no
};

/**
 * @brief A simple yes/no prompt, requires the user to press the ENTER key to continue. The arguments are used like this: 1 [2/3]4 <user Input>.
 * The immediate switch indicates toggles whether pressing enter for confirming the input is required or not.
 *
 * @param message
 * @param first_option
 * @param second_option
 * @param prompt_indicator
 * @return Result
 */
Result prompt(const std::string& message, const std::string& first_option, const std::string& second_option, const std::string& prompt_indicator, bool);

// indicates the results of prompt_simple()
enum class Result_simple
{

  Yes,   ///< Returned if the user chose \b yes.
  No,    ///< Returned if the user chose no or invalid / no input or if no terminal is attached.
  Abort  ///< Returned if CTRL+C was pressed.
};

/**
 * @brief The most simple prompt possible, requires the user to press enter to continue. The arguments are used like this: 1 [y/N]:
 * Invalid input, errors (like no attached terminal) all result in \b no as default.
 *
 * @param message
 * @return Result_simple
 */
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
