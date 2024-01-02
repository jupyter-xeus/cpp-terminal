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

namespace Term
{

class IOStreamInitializer
{
public:
  ~IOStreamInitializer() noexcept;
  IOStreamInitializer() noexcept;
  IOStreamInitializer(const IOStreamInitializer&)            = delete;
  IOStreamInitializer(IOStreamInitializer&&)                 = delete;
  IOStreamInitializer& operator=(IOStreamInitializer&&)      = delete;
  IOStreamInitializer& operator=(const IOStreamInitializer&) = delete;

private:
  static std::size_t m_counter;
};

}  // namespace Term
