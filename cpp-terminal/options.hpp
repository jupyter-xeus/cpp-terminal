/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace Term
{

///
/// @brief Option to set-up the terminal.
///
enum class Option : std::int16_t
{
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
  Options(const std::initializer_list<Term::Option>& option);
  template<typename... Args> explicit Options(const Args&&... args) : m_Options(std::initializer_list<Term::Option>{args...}) { clean(); }

  bool operator==(const Options& options);
  bool operator!=(const Options& options);
  bool has(const Option& option) const noexcept;

private:
  void                      clean();
  std::vector<Term::Option> m_Options;
};

}  // namespace Term
