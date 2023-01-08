#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/base.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <stdexcept>

#ifdef _WIN32
__pragma(warning(push)) __pragma(warning(disable : 4514)) __pragma(warning(disable : 4710)) __pragma(warning(disable : 4668))
  #include <windows.h>
  __pragma(warning(pop))
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
  static DWORD dwOriginalOutMode{};
  static UINT  out_code_page{};
  static DWORD dwOriginalInMode{};
  static UINT  in_code_page{};
  if(!enabled)
  {
    if(Term::is_stdout_a_tty())
    {
      out_code_page = GetConsoleOutputCP();
      if(!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwOriginalOutMode)) { throw std::runtime_error("GetConsoleMode() failed"); }
    }
    if(Term::is_stdin_a_tty())
    {
      in_code_page = GetConsoleCP();
      if(!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwOriginalInMode)) { throw std::runtime_error("GetConsoleMode() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(Term::is_stdout_a_tty())
    {
      SetConsoleOutputCP(out_code_page);
      if(!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwOriginalOutMode)) { throw std::runtime_error("SetConsoleMode() failed in destructor"); }
    }
    if(Term::is_stdin_a_tty())
    {
      SetConsoleCP(in_code_page);
      if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwOriginalInMode)) { throw std::runtime_error("SetConsoleMode() failed in destructor"); }
    }
  }
#else
  static termios orig_termios;
  if(!enabled)
  {
    if(Term::is_stdin_a_tty())
    {
      if(tcgetattr(0, &orig_termios) == -1) { throw std::runtime_error("tcgetattr() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(Term::is_stdin_a_tty())
    {
      if(tcsetattr(0, TCSAFLUSH, &orig_termios) == -1) { throw std::runtime_error("tcsetattr() failed in destructor"); }
    }
  }
#endif
}

Term::Terminal::Terminal(bool _clear_screen, bool enable_keyboard, bool disable_signal_keys, bool _hide_cursor) : keyboard_enabled{enable_keyboard}, disable_signal_keys{disable_signal_keys}, clear_screen{_clear_screen}, hide_cursor{_hide_cursor}
{
  store_and_restore();
#ifdef _WIN32
  // silently disable raw mode for non-tty
  if(keyboard_enabled) keyboard_enabled = Term::is_stdin_a_tty();
  if(Term::is_stdout_a_tty())
  {
    SetConsoleOutputCP(65001);
    DWORD flags{0};
    if(!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &flags)) { throw std::runtime_error("GetConsoleMode() failed"); }
    if(Term::Private::has_ansi_escape_code())
    {
      flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      flags |= DISABLE_NEWLINE_AUTO_RETURN;
    }
    if(!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), flags)) { throw std::runtime_error("SetConsoleMode() failed"); }
  }

  if(keyboard_enabled)
  {
    SetConsoleCP(65001);
    DWORD flags{0};
    if(!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &flags)) { throw std::runtime_error("GetConsoleMode() failed"); }
    if(Term::Private::has_ansi_escape_code()) { flags |= ENABLE_VIRTUAL_TERMINAL_INPUT; }
    if(disable_signal_keys) { flags &= ~ENABLE_PROCESSED_INPUT; }
    flags &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), flags)) { throw std::runtime_error("SetConsoleMode() failed"); }
  }
#else
  // silently disable raw mode for non-tty
  if(keyboard_enabled) keyboard_enabled = Term::is_stdin_a_tty();
  if(keyboard_enabled)
  {
    termios raw{};
    if(tcgetattr(0, &raw) == -1) { throw std::runtime_error("tcgetattr() failed"); }
    // Put terminal in raw mode
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // This disables output post-processing, requiring explicit \r\n. We
    // keep it enabled, so that in C++, one can still just use std::endl
    // for EOL instead of "\r\n".
    // raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    if(disable_signal_keys) { raw.c_lflag &= ~(ISIG); }
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSAFLUSH, &raw) == -1) { throw std::runtime_error("tcsetattr() failed"); }
  }
#endif
  if(clear_screen)
  {
    // Fix consoles that ignore save_screen()
    std::cout << screen_save() << clear_buffer() << style(Style::RESET) << cursor_move(1, 1);
  }
  if(hide_cursor) std::cout << cursor_off();
  // flush stdout
  std::cout << std::flush;
}

Term::Terminal::~Terminal()
{
  if(clear_screen)
  {
    // Fix consoles that ignore save_screen()
    std::cout << clear_buffer() << style(Style::RESET) << cursor_move(1, 1) << screen_load();
  }
  if(hide_cursor) std::cout << cursor_on();
  // flush the output stream
  std::cout << std::flush;
  store_and_restore();
}
