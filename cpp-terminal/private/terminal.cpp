#include "cpp-terminal/terminal.hpp"

#include "cpp-terminal/base.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <stdexcept>

#ifdef _WIN32
  // The most slimmed-down version of Windows.h.
  #define WIN32_LEAN_AND_MEAN
  #define WIN32_EXTRA_LEAN

  // Enable components based on necessity.
  #define NOGDICAPMASKS
  #define NOVIRTUALKEYCODES
  #define NOWINMESSAGES
  #define NOWINSTYLES
  #define NOSYSMETRICS
  #define NOMENUS
  #define NOICONS
  #define NOKEYSTATES
  #define NOSYSCOMMANDS
  #define NORASTEROPS
  #define NOSHOWWINDOW
  #define OEMRESOURCE
  #define NOATOM
  #define NOCLIPBOARD
  #define NOCOLOR
  #define NOCTLMGR
  #define NODRAWTEXT
  #define NOGDI
  #define NOKERNEL
  #define NOUSER
  #define NONLS
  #define NOMB
  #define NOMEMMGR
  #define NOMETAFILE
  #define NOMINMAX
  #define NOMSG
  #define NOOPENFILE
  #define NOSCROLL
  #define NOSERVICE
  #define NOSOUND
  #define NOTEXTMETRIC
  #define NOWH
  #define NOWINOFFSETS
  #define NOCOMM
  #define NOKANJI
  #define NOHELP
  #define NOPROFILER
  #define NODEFERWINDOWPOS
  #define NOMCX
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
  #include <unistd.h>
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
      if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) { throw std::runtime_error("tcgetattr() failed"); }
    }
    enabled = true;
  }
  else
  {
    if(Term::is_stdin_a_tty())
    {
      if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) { throw std::runtime_error("tcsetattr() failed in destructor"); }
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
    if(GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed"); }
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
    if(GetStdHandle(STD_INPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed"); }
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
  termios raw{};
  if(keyboard_enabled)
  {
    if(tcgetattr(STDIN_FILENO, &raw) == -1) { throw std::runtime_error("tcgetattr() failed"); }

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

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { throw std::runtime_error("tcsetattr() failed"); }
  }
#endif
  if(clear_screen)
  {
    std::cout << screen_save() + clear_buffer();  // Fix consoles that ignore save_screen()
  }
  if(hide_cursor) std::cout << cursor_off();

  // flush stdout
  std::cout << std::flush;
}
Term::Terminal::Terminal(bool _clear_screen) : keyboard_enabled{false}, disable_signal_keys{true}, clear_screen{_clear_screen}
{
  store_and_restore();
#ifdef _WIN32
  // silently disable raw mode for non-tty
  if(keyboard_enabled) keyboard_enabled = Term::is_stdin_a_tty();
  if(Term::is_stdout_a_tty())
  {
    SetConsoleOutputCP(65001);
    if(GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw std::runtime_error("GetStdHandle(STD_OUTPUT_HANDLE) failed"); }
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
    if(GetStdHandle(STD_INPUT_HANDLE) == INVALID_HANDLE_VALUE) { throw std::runtime_error("GetStdHandle(STD_INPUT_HANDLE) failed"); }
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
  termios raw{};
  if(keyboard_enabled)
  {
    if(tcgetattr(STDIN_FILENO, &raw) == -1) { throw std::runtime_error("tcgetattr() failed"); }

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

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { throw std::runtime_error("tcsetattr() failed"); }
  }
#endif
  if(clear_screen) { std::cout << screen_save() << clear_buffer() << std::flush; }
}

Term::Terminal::~Terminal()
{
  if(clear_screen)
  {
    // Fix consoles that ignore save_screen()
    std::cout << style(Style::RESET) << clear_buffer() << cursor_move(1, 1) << screen_load();
  }
  if(hide_cursor) std::cout << cursor_on();

  // flush the output stream
  std::cout << std::flush;
  store_and_restore();
}
