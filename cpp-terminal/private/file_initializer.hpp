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

namespace Private
{

class FileInitializer
{
public:
  ~FileInitializer();
  FileInitializer();
  FileInitializer(FileInitializer&&)                 = delete;
  FileInitializer(const FileInitializer&)            = delete;
  FileInitializer& operator=(const FileInitializer&) = delete;
  FileInitializer& operator=(FileInitializer&&)      = delete;

private:
  static bool        m_consoleCreated;
  static std::size_t m_counter;
  static void        attachConsole();
  static void        detachConsole();
};

}  // namespace Private

}  // namespace Term
