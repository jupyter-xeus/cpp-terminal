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
  FileInitializer();
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
  bool   null();
  FILE*  file();
  int    fd();

private:
  bool   m_null{false};
  Handle m_handle{nullptr};
  FILE*  m_file{nullptr};
  int    m_fd{-1};
};

class OutputFileHandler : public FileHandler
{
public:
  OutputFileHandler(const std::string& file, const std::string& mode = "w") : FileHandler(file, mode) {}
  void write(const std::string& str);
};

class InputFileHandler : public FileHandler
{
public:
  InputFileHandler(const std::string& file, const std::string& mode = "r") : FileHandler(file, mode) {}
};

// Even in namespace it can't be called stdin because stdin can be a Macro :(
extern InputFileHandler&  in;
extern OutputFileHandler& out;

}  // namespace Private

}  // namespace Term
