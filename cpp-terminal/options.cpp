/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/options.hpp"

#include <algorithm>

void Term::Options::set(const Term::Option& option) { m_Options.push_back(option); }

// Return true is the option is set and not its opposite (* + No* = false)
bool Term::Options::has(const Option& option)
{
  if(std::find(m_Options.begin(), m_Options.end(), option) != m_Options.end() && std::find(m_Options.begin(), m_Options.end(), static_cast<Option>(-1 * static_cast<std::int16_t>(option))) == m_Options.end()) return true;
  else
    return false;
}
