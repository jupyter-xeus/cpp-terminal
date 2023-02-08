#include "cpp-terminal/base.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

Term::Terminal::Terminal(const bool& _clear_screen,const bool& _keyboard_enabled,const bool& _disable_signal_keys,const bool& _hide_cursor) : clear_screen{_clear_screen}, keyboard_enabled{_keyboard_enabled}, disable_signal_keys{_disable_signal_keys}, hide_cursor{_hide_cursor}
{
  store_and_restore();
  // silently disable raw mode for non-tty
  if(keyboard_enabled) keyboard_enabled = Term::is_stdin_a_tty();
  setRawMode();
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
