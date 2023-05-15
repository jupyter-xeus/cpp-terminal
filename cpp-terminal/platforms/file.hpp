// Open file for stdin, stdout, stderr without redirection. A kind of FILE class for consoles
// PRIVATE !!!
#pragma once

namespace Term
{

namespace Private
{

#if defined(_WIN32)
typedef void* consoleFileHandler;
#else
typedef int consoleFileHandler;
#endif

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
  FileHandler(const char*, const char*);
  ~FileHandler();
  consoleFileHandler getHandler();
  bool               isNull();

private:
  bool  m_isNull{false};
  void* m_file{nullptr};
};

// Even in namespace it can't be called stdin because stdin can be a Macro :(
extern FileHandler& std_cin;
extern FileHandler& std_cout;

}  // namespace Private

}  // namespace Term
