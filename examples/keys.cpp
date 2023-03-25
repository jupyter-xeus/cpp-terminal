/*
 * This example shows how to interpret keys presses from Terminal.
 */
#include "cpp-terminal/base.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <utility>

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    if(!Term::is_stdin_a_tty())
    {
      std::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Term::Terminal                      term(true, true, false);
    std::pair<std::size_t, std::size_t> term_size{Term::get_size()};
    std::cout << Term::cursor_move(1, 1);
    std::cout << "Dimension:" << std::get<1>(term_size) << " " << std::get<0>(term_size) << std::endl;
    std::cout << "Press any key ( 3 time 'q' to quit):" << std::endl;
    int quit{0};
    while(quit != 3)
    {
      Term::Key key{static_cast<Term::Key::Value>(Term::read_key())};
      if(key == Term::Key::Value::q) quit++;
      else
        quit = 0;
      std::string s;
      if(key >= Term::Key::a && key <= Term::Key::z)
      {
        s = (char)(key + Term::Key::A - Term::Key::a);  // Convert to upper case
      }
      else if(key >= Term::Key::A && key <= Term::Key::Z)
      {
        s = (std::string) "Shift+" + (char)key;  // Already in upper case;
      }
      else if(key.is_CTRL()) { s = (std::string) "CTRL+" + (char)(-Term::Key::CTRL + key); }
      else if(key.is_ALT())
      {
        s = (std::string) "Alt+" + (char)(key + Term::Key::A - (Term::Key::ALT + Term::Key::a));  // Convert to upper case
      }
      else if(key.is_extended_ASCII() && !iscntrl(key)) { s = (char)key; }
      else
      {
        switch(key)
        {
          case Term::Key::BACKSPACE: s = "BACKSPACE"; break;
          case Term::Key::ENTER: s = "ENTER"; break;
          case Term::Key::ALT_ENTER: s = "Alt+ENTER"; break;
          case Term::Key::TAB: s = "TAB"; break;
          case Term::Key::ARROW_LEFT: s = "ARROW_LEFT"; break;
          case Term::Key::ARROW_RIGHT: s = "ARROW_RIGHT"; break;
          case Term::Key::ARROW_UP: s = "ARROW_UP"; break;
          case Term::Key::ARROW_DOWN: s = "ARROW_DOWN"; break;
          case Term::Key::CTRL_UP: s = "CTRL_UP"; break;
          case Term::Key::CTRL_DOWN: s = "CTRL_DOWN"; break;
          case Term::Key::CTRL_RIGHT: s = "CTRL_RIGHT"; break;
          case Term::Key::CTRL_LEFT: s = "CTRL_LEFT"; break;
          case Term::Key::NUMERIC_5: s = "NUMERIC_5"; break;
          case Term::Key::DEL: s = "DEL"; break;
          case Term::Key::HOME: s = "HOME"; break;
          case Term::Key::INSERT: s = "INSERT"; break;
          case Term::Key::END: s = "END"; break;
          case Term::Key::PAGE_UP: s = "PAGE_UP"; break;
          case Term::Key::PAGE_DOWN: s = "PAGE_DOWN"; break;
          case Term::Key::ESC: s = "ESC"; break;
          case Term::Key::F1: s = "F1"; break;
          case Term::Key::F2: s = "F2"; break;
          case Term::Key::F3: s = "F3"; break;
          case Term::Key::F4: s = "F4"; break;
          case Term::Key::F5: s = "F5"; break;
          case Term::Key::F6: s = "F6"; break;
          case Term::Key::F7: s = "F7"; break;
          case Term::Key::F8: s = "F8"; break;
          case Term::Key::F9: s = "F9"; break;
          case Term::Key::F10: s = "F10"; break;
          case Term::Key::F11: s = "F11"; break;
          case Term::Key::F12: s = "F12"; break;

          default: s = "Unknown:" + std::to_string(key);
        }
      }
      std::cout << "Key: " << s << std::endl;
    }
  }
  catch(const Term::Exception& re)
  {
    std::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
