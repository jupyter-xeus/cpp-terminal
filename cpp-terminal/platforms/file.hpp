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
  Handle getHandler();
  bool   isNull();
  FILE*  file();
  int    fd();

private:
  bool   m_isNull{false};
  Handle m_handle{nullptr};
  FILE*  m_file{nullptr};
  int    m_fd{-1};
};

// Even in namespace it can't be called stdin because stdin can be a Macro :(
extern FileHandler& std_cin;
extern FileHandler& std_cout;

}  // namespace Private

}  // namespace Term
