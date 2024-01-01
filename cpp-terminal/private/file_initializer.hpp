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

namespace Private
{

class FileInitializer
{
public:
  ~FileInitializer() noexcept;
  FileInitializer() noexcept;
  FileInitializer(FileInitializer&&)                 = delete;
  FileInitializer(const FileInitializer&)            = delete;
  FileInitializer& operator=(const FileInitializer&) = delete;
  FileInitializer& operator=(FileInitializer&&)      = delete;

private:
  static bool        m_consoleCreated;
  static std::size_t m_counter;

  ///
  ///@brief Attach the console.
  ///
  ///Check if a console is attached to the process. If not, try to attach to the console. If there is no console, then create one.
  ///
  static void attachConsole() noexcept;

  ///
  ///@brief Detach the console.
  ///
  ///If a console as been created, then delete it.
  ///
  static void detachConsole() noexcept;

  ///
  ///@brief Open the standard streams.
  ///
  ///Open \b stdout \b stderr \b stdin and adjust their buffer size and line discipline.
  ///
  static void openStandardStreams() noexcept;
};

}  // namespace Private

}  // namespace Term
