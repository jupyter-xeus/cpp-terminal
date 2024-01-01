/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/private/file_initializer.hpp"

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

namespace Term
{

namespace Private
{

class FileHandler
{
public:
#if defined(_WIN32)
  using Handle = void*;
#else
  using Handle = std::FILE*;
#endif
  FileHandler(std::recursive_mutex& mutex, const std::string& file, const std::string& mode) noexcept;
  FileHandler(const FileHandler&)            = delete;
  FileHandler(FileHandler&&)                 = delete;
  FileHandler& operator=(const FileHandler&) = delete;
  FileHandler& operator=(FileHandler&&)      = delete;
  virtual ~FileHandler() noexcept;
  Handle       handle() const noexcept;
  bool         null() const noexcept;
  std::FILE*   file() const noexcept;
  std::int32_t fd() const noexcept;
  void         lockIO();
  void         unlockIO();
  void         flush();

private:
  // should be static but MacOS don't want it (crash at runtime)
  std::recursive_mutex& m_mutex;  //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
  bool                  m_null{false};
  Handle                m_handle{nullptr};
  FILE*                 m_file{nullptr};
  std::int32_t          m_fd{-1};
};

class OutputFileHandler : public FileHandler
{
public:
  explicit OutputFileHandler(std::recursive_mutex& io_mutex) noexcept;
  OutputFileHandler(const OutputFileHandler& other)          = delete;
  OutputFileHandler(OutputFileHandler&& other)               = delete;
  OutputFileHandler& operator=(OutputFileHandler&& rhs)      = delete;
  OutputFileHandler& operator=(const OutputFileHandler& rhs) = delete;
  ~OutputFileHandler() override                              = default;

  std::size_t write(const std::string& str) const;
  std::size_t write(const char& character) const;
#if defined(_WIN32)
  static const constexpr char* m_file{"CONOUT$"};
#else
  static const constexpr char* m_file{"/dev/tty"};
#endif
};

class InputFileHandler : public FileHandler
{
public:
  explicit InputFileHandler(std::recursive_mutex& io_mutex) noexcept;
  InputFileHandler(const InputFileHandler&)            = delete;
  InputFileHandler(InputFileHandler&&)                 = delete;
  InputFileHandler& operator=(InputFileHandler&&)      = delete;
  InputFileHandler& operator=(const InputFileHandler&) = delete;
  ~InputFileHandler() override                         = default;

  std::string read() const;
#if defined(_WIN32)
  static const constexpr char* m_file{"CONIN$"};
#else
  static const constexpr char* m_file{"/dev/tty"};
#endif
};

extern InputFileHandler&     in;
extern OutputFileHandler&    out;
static const FileInitializer file_initializer;

std::string ask(const std::string& str);

}  // namespace Private

}  // namespace Term
