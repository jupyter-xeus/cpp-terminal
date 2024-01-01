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

namespace Term
{
namespace Private
{

class Sigwinch
{
public:
  static void         registerSigwinch();
  static void         blockSigwinch();
  static void         unblockSigwinch();
  static bool         isSigwinch(const std::int32_t& file_descriptor = -1) noexcept;
  static std::int32_t get() noexcept;

private:
  static std::int32_t m_fd;
};

}  // namespace Private
}  // namespace Term
