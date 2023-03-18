#include "cpp-terminal/terminal.hpp"

#ifdef _WIN32
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
  #include <termios.h>
#endif

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/tty.hpp"

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
  if(!enabled)
  {
    if(Term::is_stdin_a_tty())
    {
      if(tcgetattr(0, &orig_termios) == -1) { throw Term::Exception("tcgetattr() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(Term::is_stdin_a_tty())
    {
      if(tcsetattr(0, TCSAFLUSH, &orig_termios) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
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
  DWORD flags{0};
  if(!GetConsoleMode(hConIn, &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_terminfo.hasANSIEscapeCode()) { flags |= ENABLE_VIRTUAL_TERMINAL_INPUT; }
  if(disable_signal_keys) { flags &= ~ENABLE_PROCESSED_INPUT; }
  flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  if(!SetConsoleMode(hConIn, flags)) { throw Term::Exception("SetConsoleMode() failed"); }
  CloseHandle(hConOut);
  CloseHandle(hConIn);
#else
  int i{-1};
  if(Term::is_stdin_a_tty()) i = 0;
  else if(Term::is_stdout_a_tty())
    i = 1;
  else if(Term::is_stderr_a_tty())
    i = 2;
  if(i >= 0)
  {
    termios raw{};
    if(tcgetattr(i, &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
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
    if(tcsetattr(i, TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
  }
#endif
}
