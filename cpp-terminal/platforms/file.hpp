// Open file for stdin, stdout, stderr without redirection. A kind of FILE class for consoles
// PRIVATE !!!
#pragma once

#include <string>

namespace Term
{

namespace Private
{

class FileInitializer
{
public:
  FileInitializer() = default;
  void initialize();
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
  FileHandler(const std::string&, const std::string&);
  ~FileHandler();
  Handle handle();
  bool   null() const;
  FILE*  file();
  int    fd() const;

private:
  bool   m_null{false};
  Handle m_handle{nullptr};
  FILE*  m_file{nullptr};
  int    m_fd{-1};
};

class OutputFileHandler : public FileHandler
{
public:
  explicit OutputFileHandler(const std::string& file, const std::string& mode = "w") : FileHandler(file, mode) {}
  int write(const std::string& str);
};

class InputFileHandler : public FileHandler
{
public:
  explicit InputFileHandler(const std::string& file, const std::string& mode = "r") : FileHandler(file, mode) {}
};

extern InputFileHandler&  in;
extern OutputFileHandler& out;

}  // namespace Private

}  // namespace Term
