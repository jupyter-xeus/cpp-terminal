/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstddef>

namespace Term
{

class StreamInitializer
{
public:
  StreamInitializer();
  static void init();
  ~StreamInitializer();
  StreamInitializer(const StreamInitializer&)            = delete;
  StreamInitializer& operator=(const StreamInitializer&) = delete;
  StreamInitializer(StreamInitializer&&)                 = delete;
  StreamInitializer& operator=(StreamInitializer&&)      = delete;

private:
  static std::size_t m_counter;
};

}  // namespace Term
