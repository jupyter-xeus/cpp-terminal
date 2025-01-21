/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <cstddef>
#include <vector>

using sighandler_t = void (*)(int);

namespace Term
{

class Terminal;

namespace Private
{

class Signals
{
public:
  Signals(const Terminal&) noexcept;
  ~Signals() noexcept = default;
  void        setHandler(const sighandler_t& handler) noexcept;
  void        reset_and_raise(const int& sign) noexcept;
  static void clean_terminal() noexcept;

private:
  const static std::size_t     m_signals_number;
  std::vector<sighandler_t>    m_handlers;
  const static Term::Terminal* m_term;
};

}  // namespace Private

}  // namespace Term
