/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/options.hpp"

#include <algorithm>

Term::Options::Options(const std::initializer_list<Term::Option>& option) : m_Options(option) { clean(); }

bool Term::Options::operator==(const Options& options) { return m_Options == options.m_Options; }
bool Term::Options::operator!=(const Options& options) { return !(m_Options == options.m_Options); }

void Term::Options::clean()
{
  std::vector<Term::Option> cleaned;
  std::sort(m_Options.begin(), m_Options.end());
  while(!m_Options.empty())
  {
    const std::size_t count      = std::count(m_Options.begin(), m_Options.end(), m_Options[0]);
    const std::size_t anti_count = std::count(m_Options.begin(), m_Options.end(), static_cast<Term::Option>(-1 * static_cast<std::int16_t>(m_Options[0])));
    if(count > anti_count) { cleaned.emplace_back(m_Options[0]); }
    else if(count < anti_count) { cleaned.emplace_back(static_cast<Term::Option>(-1 * static_cast<std::int16_t>(m_Options[0]))); }
    m_Options.erase(std::remove(m_Options.begin(), m_Options.end(), static_cast<Term::Option>(-1 * static_cast<std::int16_t>(m_Options[0]))), m_Options.end());
    m_Options.erase(std::remove(m_Options.begin(), m_Options.end(), m_Options[0]), m_Options.end());
  }
  m_Options = cleaned;
}

bool Term::Options::has(const Option& option) const noexcept { return std::find(m_Options.begin(), m_Options.end(), option) != m_Options.end(); }
