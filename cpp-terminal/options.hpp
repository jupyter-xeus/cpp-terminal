/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <vector>

namespace Term
{

/**
 * @brief Option to set-up the terminal.
 *
 */
enum class Option : std::int16_t
{
  Default       = 0,   ///< Default state set up by cpp-terminal.
  Raw           = 1,   ///< Set terminal in \b raw mode.
  Cooked        = -1,  ///< Set terminal in \b cooked mode.
  ClearScreen   = 2,   ///< Clear the screen (and restore its states when the program stops).
  NoClearScreen = -2,  ///< Doesn't clear the screen.
  SignalKeys    = 3,   ///< Enable the signal keys (Ctrl+C, etc...), if activated these keys will have their default OS behaviour.
  NoSignalKeys  = -3,  ///< Disable the signal keys (Ctrl+C, etc...) will not be processed by the OS and will appears has standard combination keys.
  Cursor        = 4,   ///< Show the cursor.
  NoCursor      = -4   ///< Hide the cursor (and restore its states when the program stops).
};

class Options
{
public:
  Options() = default;
  bool has(const Option& option);
  void set(const Option& option);

private:
  std::vector<Option> m_Options;
};

}  // namespace Term
