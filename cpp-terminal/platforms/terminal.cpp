#include "cpp-terminal/terminal.hpp"

#ifdef _WIN32
  #include <io.h>
  #include <share.h>
  #include <windows.h>
  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
  #ifndef DISABLE_NEWLINE_AUTO_RETURN
    #define DISABLE_NEWLINE_AUTO_RETURN 0x0008
  #endif
  #ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
    #define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
  #endif
#else
  #include <fcntl.h>
  #include <termios.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/exception.hpp"

#include <fstream>

void Term::Terminal::store_and_restore()
{
  static bool enabled{false};
#ifdef _WIN32
  static UINT  out_code_page{0};
  static UINT  in_code_page{0};
  static DWORD dwOriginalOutMode{0};
  static DWORD dwOriginalInMode{0};
  if(!enabled)
  {
    out_code_page = GetConsoleOutputCP();
    if(out_code_page == 0) throw Term::Exception("GetConsoleOutputCP() failed");
    in_code_page = GetConsoleCP();
    if(out_code_page == 0) throw Term::Exception("GetConsoleCP() failed");
    if(!SetConsoleOutputCP(CP_UTF8)) throw Term::Exception("SetConsoleOutputCP(CP_UTF8) failed");
    if(!SetConsoleCP(CP_UTF8)) throw Term::Exception("SetConsoleCP(CP_UTF8) failed");

    HANDLE hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    if(hConOut == INVALID_HANDLE_VALUE) throw Term::Exception("CreateFile failed");
    if(!GetConsoleMode(hConOut, &dwOriginalOutMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    if(m_terminfo.hasANSIEscapeCode())
    {
      if(!SetConsoleMode(hConOut, dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    }
    CloseHandle(hConOut);

    HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    if(hConIn == INVALID_HANDLE_VALUE) throw Term::Exception("CreateFile failed");
    if(!GetConsoleMode(hConIn, &dwOriginalInMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    if(m_terminfo.hasANSIEscapeCode())
    {
      if(!SetConsoleMode(hConIn, dwOriginalInMode | ENABLE_VIRTUAL_TERMINAL_INPUT)) { throw Term::Exception("SetConsoleMode() failed"); }
    }
    CloseHandle(hConIn);

    enabled = true;
  }
  else
  {
    if(!SetConsoleOutputCP(out_code_page)) throw Term::Exception("SetConsoleOutputCP(out_code_page) failed");
    if(!SetConsoleCP(in_code_page)) throw Term::Exception("SetConsoleCP(in_code_page) failed");
    HANDLE hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    if(hConOut == INVALID_HANDLE_VALUE) throw Term::Exception("CreateFile failed");
    if(!SetConsoleMode(hConOut, dwOriginalOutMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    CloseHandle(hConOut);
    HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    if(hConIn == INVALID_HANDLE_VALUE) throw Term::Exception("CreateFile failed");
    if(!SetConsoleMode(hConIn, dwOriginalInMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    CloseHandle(hConIn);
    enabled = false;
  }
#else
  static termios orig_termios;
  if(!enabled)
  {
    int fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
    if(fd >= 0)
    {
      if(tcgetattr(fd, &orig_termios) == -1) { throw Term::Exception("tcgetattr() failed"); }
    }
    close(fd);
    enabled = true;
  }
  else
  {
    int fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
    if(fd >= 0)
    {
      if(tcsetattr(fd, TCSAFLUSH, &orig_termios) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
      close(fd);
    }
    enabled = false;
  }
#endif
}

void Term::Terminal::setRawMode()
{
#ifdef _WIN32
  HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  DWORD  flags = {0};
  if(!GetConsoleMode(hConIn, &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_options.has(Option::NoSignalKeys)) { flags &= ~ENABLE_PROCESSED_INPUT; }
  flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  if(!SetConsoleMode(hConIn, flags)) { throw Term::Exception("SetConsoleMode() failed"); }
  CloseHandle(hConIn);
#else
  int fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
  if(fd >= 0)
  {
    termios raw;
    if(tcgetattr(fd, &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
    // Put terminal in raw mode
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // This disables output post-processing, requiring explicit \r\n. We
    // keep it enabled, so that in C++, one can still just use std::endl
    // for EOL instead of "\r\n".
    // raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    if(m_options.has(Option::NoSignalKeys)) { raw.c_lflag &= ~ISIG; }
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    if(tcsetattr(fd, TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
    close(fd);
  }
#endif
}

void Term::Terminal::attachConsole()
{
#ifdef _WIN32
  FILE* dump{nullptr};
  if(!AttachConsole(ATTACH_PARENT_PROCESS))
  {
    if(AllocConsole())
    {
      AttachConsole(GetCurrentProcessId());
      freopen_s(&dump, "CONOUT$", "w", stdout);
      freopen_s(&dump, "CONOUT$", "w", stderr);
      freopen_s(&dump, "CONIN$", "r", stdin);
      has_allocated_console = true;
    }
  }
  else
  {
    if(_fileno(stdout) < 0 || _get_osfhandle(_fileno(stdout)) < 0) freopen_s(&dump, "CONOUT$", "w", stdout);
    if(_fileno(stderr) < 0 || _get_osfhandle(_fileno(stderr)) < 0) freopen_s(&dump, "CONOUT$", "w", stderr);
    if(_fileno(stdin) < 0 || _get_osfhandle(_fileno(stdin)) < 0) freopen_s(&dump, "CONIN$", "r", stdin);
  }
#endif
}

void Term::Terminal::attachStreams()
{
#if defined(_WIN32)
  std::string in{"CONIN$"};
  std::string out{"CONOUT$"};
  std::string blackHole{"NUL"};
#else
  std::string in{"/dev/tty"};
  std::string out{"/dev/tty"};
  std::string blackHole{"/dev/null"};
#endif
  this->cout.open(out.c_str(), std::ofstream::out | std::ofstream::trunc);
  if(!this->cout.is_open()) this->cout.open(blackHole.c_str(), std::ofstream::out | std::ofstream::trunc);
  this->cout.clear();
  this->cerr.open(out.c_str(), std::ofstream::out | std::ofstream::trunc);
  if(!this->cerr.is_open()) this->cerr.open(blackHole.c_str(), std::ofstream::out | std::ofstream::trunc);
  this->cerr.clear();
  this->clog.rdbuf()->pubsetbuf(nullptr, 0);
  this->clog.open(out.c_str(), std::ofstream::out | std::ofstream::trunc);
  if(!this->clog.is_open()) this->clog.open(blackHole.c_str(), std::ofstream::out | std::ofstream::trunc);
  this->clog.rdbuf()->pubsetbuf(nullptr, 0);
  this->clog.clear();
  this->cin.open(in.c_str(), std::ofstream::in);
  if(!this->cin.is_open()) this->cin.open(blackHole.c_str(), std::ofstream::in);
  this->cin.clear();
  cout.clear();
  cerr.clear();
  clog.clear();
  cin.clear();
}

void Term::Terminal::detachStreams()
{
  if(cout.is_open()) cout.close();
  if(cerr.is_open()) cerr.close();
  if(clog.is_open()) clog.close();
  if(cin.is_open()) cin.close();
}

void Term::Terminal::detachConsole()
{
#ifdef _WIN32
  if(has_allocated_console) FreeConsole();
#else
#endif
}
