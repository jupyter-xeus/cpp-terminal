/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
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

enum class Option : int
{
  // Don't use 0!
  Default       = 0,
  Raw           = 1,
  Cooked        = -1,
  ClearScreen   = 2,
  NoClearScreen = -2,
  SignalKeys    = 3,
  NoSignalKeys  = -3,
  Cursor        = 4,
  NoCursor      = -4
};

class Options
{
public:
  Options() = default;
  bool has(const Option&);
  void set(const Option&);

private:
  std::vector<Option> m_Options;
};

}  // namespace Term
