#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"
#include "cpp-terminal/version.hpp"

#include <iostream>

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
    Term::terminal.setOptions({Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw});

    Term::Cursor cursor{Term::cursor_position()};
    std::cout << "Cursor position : " << cursor.row() << " " << cursor.column() << std::endl;

    Term::Screen term_size{Term::screen_size()};
    std::cout << "Dimension:" << term_size.columns() << " " << term_size.rows() << std::endl;
    std::cout << "Press any key ( 3 time 'q' to quit):" << std::endl;
    int quit{0};
    while(quit != 3)
    {
      Term::Event event{Term::read_event()};
      switch(event.type())
      {
        case Term::Event::Type::Key:
        {
          Term::Key key(event);
          if(key == Term::Key::Value::q) quit++;
          else
            quit = 0;
          std::string s;
          if(key == Term::Key::Value::NO_KEY) continue;
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
              case Term::Key::ALT + Term::Key::ENTER: s = "Alt+ENTER"; break;
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
              case Term::Key::DEL: s = "DELETE"; break;
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
              case Term::Key::F13: s = "F13"; break;
              case Term::Key::F14: s = "F14"; break;
              case Term::Key::F15: s = "F15"; break;
              case Term::Key::F16: s = "F16"; break;
              case Term::Key::F17: s = "F17"; break;
              case Term::Key::F18: s = "F18"; break;
              case Term::Key::F19: s = "F19"; break;
              case Term::Key::F20: s = "F20"; break;
              case Term::Key::F21: s = "F21"; break;
              case Term::Key::F22: s = "F22"; break;
              case Term::Key::F23: s = "F23"; break;
              case Term::Key::F24: s = "F24"; break;
              default: s = "Unknown:" + std::to_string(key);
            }
          }
          std::cout << "Key: " << s << " (" << key << ") " << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste:
        {
          std::string key_str(event);
          if(!key_str.empty() && key_str[0] == '\033')
          {
            std::cout << "You discovered a key combination not yet managed by cpp-terminal (";
            for(std::size_t i = 0; i != key_str.size(); ++i) std::cout << static_cast<std::int32_t>(key_str[i]) << " ";
            std::cout << ").\nPlease report key combination pressed to " << Term::Homepage << std::endl;
          }
        }
        default: break;
      }
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
