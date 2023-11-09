/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/private/file_initializer.hpp"

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
  using Handle = FILE*;
#endif
  FileHandler(std::recursive_mutex& mutex, const std::string& file, const std::string& mode);
  FileHandler(const FileHandler&)            = delete;
  FileHandler(FileHandler&&)                 = delete;
  FileHandler& operator=(const FileHandler&) = delete;
  FileHandler& operator=(FileHandler&&)      = delete;
  virtual ~FileHandler();
  Handle       handle();
  bool         null() const;
  FILE*        file();
  std::int32_t fd() const;
  void         lockIO();
  void         unlockIO();
  void         flush();

private:
  std::recursive_mutex& m_mutex;  // should be static but MacOS don't want it (crash at runtime)
  bool                  m_null{false};
  Handle                m_handle{nullptr};
  FILE*                 m_file{nullptr};
  std::int32_t          m_fd{-1};
};

class OutputFileHandler : public FileHandler
{
public:
  explicit OutputFileHandler(std::recursive_mutex& IOmutex) : FileHandler(IOmutex, m_file, "w") {}
  int write(const std::string& str);
  int write(const char& ch);
  OutputFileHandler(const OutputFileHandler& other)          = delete;
  OutputFileHandler& operator=(const OutputFileHandler& rhs) = delete;
  OutputFileHandler(OutputFileHandler&& other)               = delete;
  OutputFileHandler& operator=(OutputFileHandler&& rhs)      = delete;
  virtual ~OutputFileHandler()                               = default;
#if defined(_WIN32)
  static const constexpr char* m_file{"CONOUT$"};
#else
  static const constexpr char* m_file{"/dev/tty"};
#endif
};

class InputFileHandler : public FileHandler
{
public:
  explicit InputFileHandler(std::recursive_mutex& IOmutex) : FileHandler(IOmutex, m_file, "r") {}
  std::string read();
  InputFileHandler(const InputFileHandler&)            = delete;
  InputFileHandler& operator=(const InputFileHandler&) = delete;
  InputFileHandler(InputFileHandler&&)                 = delete;
  InputFileHandler& operator=(InputFileHandler&&)      = delete;
  virtual ~InputFileHandler()                          = default;
#if defined(_WIN32)
  static const constexpr char* m_file{"CONIN$"};
#else
  static const constexpr char* m_file{"/dev/tty"};
#endif
};

extern InputFileHandler&     in;
extern OutputFileHandler&    out;
static const FileInitializer file_initializer;
}  // namespace Private

}  // namespace Term
