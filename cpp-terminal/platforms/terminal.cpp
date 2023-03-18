#include "cpp-terminal/terminal.hpp"

#ifdef _WIN32
  #include <io.h>
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

#include <iostream>

void Term::Terminal::store_and_restore()
{
  static bool enabled{false};
#ifdef _WIN32
  static DWORD  dwOriginalOutMode{};
  static UINT   out_code_page{};
  static DWORD  dwOriginalInMode{};
  static UINT   in_code_page{};
  static HANDLE hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  static HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  if(!enabled)
  {
    out_code_page = GetConsoleOutputCP();
    if(!GetConsoleMode(hConOut, &dwOriginalOutMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    in_code_page = GetConsoleCP();
    if(!GetConsoleMode(hConIn, &dwOriginalInMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    enabled = true;
  }
  else
  {
    SetConsoleOutputCP(out_code_page);
    if(!SetConsoleMode(hConOut, dwOriginalOutMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    SetConsoleCP(in_code_page);
    if(!SetConsoleMode(hConIn, dwOriginalInMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    CloseHandle(hConOut);
    CloseHandle(hConIn);
  }
#else
  static termios orig_termios;
  static int     fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
  if(!enabled)
  {
    if(fd >= 0)
    {
      if(tcgetattr(fd, &orig_termios) == -1) { throw Term::Exception("tcgetattr() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(fd >= 0)
    {
      if(tcsetattr(fd, TCSAFLUSH, &orig_termios) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
      close(fd);
    }
  }
#endif
}

void Term::Terminal::setRawMode()
{
#ifdef _WIN32
  HANDLE hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
  SetConsoleOutputCP(65001);
  DWORD flags{0};
  if(!GetConsoleMode(hConOut, &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_terminfo.hasANSIEscapeCode())
  {
    flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    flags |= DISABLE_NEWLINE_AUTO_RETURN;
  }
  if(!SetConsoleMode(hConOut, flags)) { throw Term::Exception("SetConsoleMode() failed"); }

  SetConsoleCP(65001);
  flags = {0};
  if(!GetConsoleMode(hConIn, &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_terminfo.hasANSIEscapeCode()) { flags |= ENABLE_VIRTUAL_TERMINAL_INPUT; }
  if(disable_signal_keys) { flags &= ~ENABLE_PROCESSED_INPUT; }
  flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  if(!SetConsoleMode(hConIn, flags)) { throw Term::Exception("SetConsoleMode() failed"); }
  CloseHandle(hConOut);
  CloseHandle(hConIn);
#else
  int fd{open("/dev/tty", O_RDWR, O_NOCTTY)};
  if(fd >= 0)
  {
    termios raw{};
    if(tcgetattr(fd, &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
    // Put terminal in raw mode
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // This disables output post-processing, requiring explicit \r\n. We
    // keep it enabled, so that in C++, one can still just use std::endl
    // for EOL instead of "\r\n".
    // raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    if(disable_signal_keys) { raw.c_lflag &= ~ISIG; }
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
  if(!AttachConsole(ATTACH_PARENT_PROCESS))
  {
    if(AllocConsole())
    {
      has_allocated_console = true;
      AttachConsole(GetCurrentProcessId());
      freopen_s(&m_stdout, "CONOUT$", "w", stdout);
      freopen_s(&m_stderr, "CONOUT$", "w", stderr);
      freopen_s(&m_stdin, "CONIN$", "r", stdin);
    }
  }
  else
  {
    HANDLE hConOut{CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    HANDLE hConIn{CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
    if(_fileno(stdout) < 0 || _get_osfhandle(_fileno(stdout)) < 0) freopen_s(&m_stdout, "CONOUT$", "w", stdout);
    else
      m_stdout = _fsopen("CONOUT$", "w", _SH_DENYNO);
    if(_fileno(stderr) < 0 || _get_osfhandle(_fileno(stderr)) < 0) freopen_s(&m_stderr, "CONOUT$", "w", stderr);
    else
      m_stderr = _fsopen("CONOUT$", "w", _SH_DENYNO);
    if(_fileno(stdin) < 0 || _get_osfhandle(_fileno(stdin)) < 0) freopen_s(&m_stdin, "CONIN$", "r", stdin);
    else
      m_stdin = _fsopen("CONIN$", "r", _SH_DENYNO);
  }

#else
#endif
  std::cout.clear();
  std::clog.clear();
  std::cerr.clear();
  std::cin.clear();
  std::wcout.clear();
  std::wclog.clear();
  std::wcerr.clear();
  std::wcin.clear();
}

void Term::Terminal::detachConsole()
{
#ifdef _WIN32
  if(has_allocated_console) FreeConsole();
#else
#endif
  if(m_stdin) fclose(m_stdin);
  if(m_stdout) fclose(m_stdout);
  if(m_stderr) fclose(m_stderr);
}
