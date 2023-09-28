/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 Ondřej Čertík
*
* SPDX-License-Identifier: MIT
*/

// Open file for stdin, stdout, stderr without redirection. A kind of FILE class for consoles
// PRIVATE !!!
#pragma once

#include <mutex>
#include <string>

namespace Term
{

namespace Private
{

class FileInitializer
{
public:
  FileInitializer();
  static void init();
  ~FileInitializer();

private:
  static int m_counter;
};

static FileInitializer m_fileInitializer;

class FileHandler
{
public:
#if defined(_WIN32)
  using Handle = void*;
#else
  using Handle = FILE*;
#endif
  FileHandler(std::recursive_mutex& mutex, const std::string&, const std::string&);
  ~FileHandler();
  Handle handle();
  bool   null() const;
  FILE*  file();
  int    fd() const;
  void   lockIO();
  void   unlockIO();
  FileHandler(const FileHandler&)            = delete;
  FileHandler& operator=(const FileHandler&) = delete;
  FileHandler(FileHandler&&)                 = delete;
  FileHandler& operator=(FileHandler&&)      = delete;

private:
  std::recursive_mutex& m_mutex;  // should be static but MacOS don't want it (crash at runtime)
  bool                  m_null{false};
  Handle                m_handle{nullptr};
  FILE*                 m_file{nullptr};
  int                   m_fd{-1};
};

class OutputFileHandler : public FileHandler
{
public:
  explicit OutputFileHandler(std::recursive_mutex& IOmutex, const std::string& file, const std::string& mode = "w") : FileHandler(IOmutex, file, mode) {}
  int write(const std::string& str);
  int write(const char& ch);
  OutputFileHandler(const OutputFileHandler& other)          = delete;
  OutputFileHandler& operator=(const OutputFileHandler& rhs) = delete;
  OutputFileHandler(OutputFileHandler&& other)               = delete;
  OutputFileHandler& operator=(OutputFileHandler&& rhs)      = delete;
};

class InputFileHandler : public FileHandler
{
public:
  explicit InputFileHandler(std::recursive_mutex& IOmutex, const std::string& file, const std::string& mode = "r") : FileHandler(IOmutex, file, mode) {}
  std::string read();
  InputFileHandler(const InputFileHandler&)            = delete;
  InputFileHandler& operator=(const InputFileHandler&) = delete;
  InputFileHandler(InputFileHandler&&)                 = delete;
  InputFileHandler& operator=(InputFileHandler&&)      = delete;
};

extern InputFileHandler&  in;
extern OutputFileHandler& out;

}  // namespace Private

}  // namespace Term
