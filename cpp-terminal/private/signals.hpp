/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
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
  Signals(std::vector<sighandler_t>& m_han) noexcept;
  ~Signals() noexcept {}
  void        setHandler(const sighandler_t& handler) noexcept;
  static void reset_and_raise(int sign, std::vector<sighandler_t>& m_han, Term::Terminal&) noexcept;

private:
  const static std::size_t m_signals_number;
};
}  // namespace Private
}  // namespace Term
