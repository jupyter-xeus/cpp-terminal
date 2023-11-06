/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>

namespace Term
{

namespace Private
{

class FileInitializer
{
public:
  FileInitializer(FileInitializer&&)      = delete;
  FileInitializer(const FileInitializer&) = delete;
  FileInitializer();
  ~FileInitializer();
  FileInitializer& operator=(const FileInitializer&) = delete;
  FileInitializer& operator=(FileInitializer&&)      = delete;
  static void      init();

private:
  static bool        m_consoleCreated;
  static std::size_t m_counter;
  static void        attachConsole();
  static void        detachConsole();
};

static const FileInitializer m_fileInitializer;

}  // namespace Private

}  // namespace Term
