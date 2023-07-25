#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/platforms/env.hpp"
#include "cpp-terminal/platforms/file.hpp"

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
  #include <termios.h>
#endif

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

    if(!GetConsoleMode(Private::out.handle(), &dwOriginalOutMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    if(m_terminfo.hasANSIEscapeCode())
    {
      if(!SetConsoleMode(Private::out.handle(), dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    }
    if(!GetConsoleMode(Private::in.handle(), &dwOriginalInMode)) { throw Term::Exception("GetConsoleMode() failed"); }
    if(m_terminfo.hasANSIEscapeCode())
    {
      if(!SetConsoleMode(Private::in.handle(), dwOriginalInMode | ENABLE_VIRTUAL_TERMINAL_INPUT)) { throw Term::Exception("SetConsoleMode() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(!SetConsoleMode(Private::out.handle(), dwOriginalOutMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    if(!SetConsoleMode(Private::in.handle(), dwOriginalInMode)) { throw Term::Exception("SetConsoleMode() failed in destructor"); }
    if(!SetConsoleOutputCP(out_code_page)) throw Term::Exception("SetConsoleOutputCP(out_code_page) failed");
    if(!SetConsoleCP(in_code_page)) throw Term::Exception("SetConsoleCP(in_code_page) failed");
    enabled = false;
  }
#else
  static termios orig_termios;
  if(!enabled)
  {
    if(!Private::out.null())
      if(tcgetattr(Private::out.fd(), &orig_termios) == -1) { throw Term::Exception("tcgetattr() failed"); }
    enabled = true;
  }
  else
  {
    if(!Private::out.null())
      if(tcsetattr(Private::out.fd(), TCSAFLUSH, &orig_termios) == -1) { throw Term::Exception("tcsetattr() failed in destructor"); }
    enabled = false;
  }
#endif
}

void Term::Terminal::setBadStateReturnCode()
{
  std::pair<bool, std::string> returnCode{Private::getenv("CPP_TERMINAL_BADSTATE")};
  try
  {
    if(returnCode.first && stoi(returnCode.second) != EXIT_SUCCESS) m_badReturnCode = stoi(returnCode.second);
  }
  catch(...)
  {
    m_badReturnCode = EXIT_FAILURE;
  }
}

void Term::Terminal::setRawMode()
{
#ifdef _WIN32
  DWORD flags = {0};
  if(!GetConsoleMode(Private::in.handle(), &flags)) { throw Term::Exception("GetConsoleMode() failed"); }
  if(m_options.has(Option::NoSignalKeys)) { flags &= ~ENABLE_PROCESSED_INPUT; }
  flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  if(!SetConsoleMode(Private::in.handle(), flags)) { throw Term::Exception("SetConsoleMode() failed"); }
#else
  if(!Private::out.null())
  {
    ::termios raw;
    if(tcgetattr(Private::out.fd(), &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
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
    if(tcsetattr(Private::out.fd(), TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
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
  setvbuf(stdin, nullptr, _IOLBF, 4096);
  setvbuf(stdout, nullptr, _IOLBF, 4096);
  setvbuf(stderr, nullptr, _IOLBF, 4096);
  Term::Private::m_fileInitializer.init();
}

void Term::Terminal::detachConsole()
{
#ifdef _WIN32
  if(has_allocated_console) FreeConsole();
#else
#endif
}
