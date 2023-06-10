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

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    Term::terminal.setOptions(Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw);
    if(!Term::is_stdin_a_tty()) throw Term::Exception("The terminal is not attached to a TTY and therefore can't catch user input. Exiting...");

    Term::Cursor cursor{Term::cursor_position()};
    Term::terminal << "Cursor position : " << cursor.row() << " " << cursor.column() << std::endl;

    Term::Screen term_size{Term::screen_size()};
    Term::terminal << "Dimension:" << term_size.columns() << " " << term_size.rows() << std::endl;
    Term::terminal << "Press any key ( 3 time 'q' to quit):" << std::endl;
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
          std::string ret;
          if(key.isCTRL())
          {
            key = Term::Key(static_cast<Term::Key::Value>(key + 64));
            ret += static_cast<std::string>("CTRL+");
          }
          if(key.isALT())
          {
            key = Term::Key(static_cast<Term::Key::Value>(key - Term::Key::Value::ALT));
            ret += static_cast<std::string>("ALT+");
          }
          switch(key)
          {
            case Term::Key::SPACE: ret += "Space"; break;
            case Term::Key::BACKSPACE: ret += "Backspace"; break;
            case Term::Key::ENTER: ret += "Enter"; break;
            case Term::Key::TAB: ret += "Tab"; break;
            case Term::Key::ARROW_LEFT: ret += "Left Arrow"; break;
            case Term::Key::ARROW_RIGHT: ret += "Right Arrow"; break;
            case Term::Key::ARROW_UP: ret += "Up arrow"; break;
            case Term::Key::ARROW_DOWN: ret += "Down arrow"; break;
            case Term::Key::NUMERIC_5: ret += "5 Numeric pad"; break;
            case Term::Key::DEL: ret += "Delete"; break;
            case Term::Key::HOME: ret += "Home"; break;
            case Term::Key::INSERT: ret += "Insert"; break;
            case Term::Key::END: ret += "End"; break;
            case Term::Key::PAGE_UP: ret += "Page up"; break;
            case Term::Key::PAGE_DOWN: ret += "Page down"; break;
            case Term::Key::ESC: ret += "Esc"; break;
            case Term::Key::F1: ret += "F1"; break;
            case Term::Key::F2: ret += "F2"; break;
            case Term::Key::F3: ret += "F3"; break;
            case Term::Key::F4: ret += "F4"; break;
            case Term::Key::F5: ret += "F5"; break;
            case Term::Key::F6: ret += "F6"; break;
            case Term::Key::F7: ret += "F7"; break;
            case Term::Key::F8: ret += "F8"; break;
            case Term::Key::F9: ret += "F9"; break;
            case Term::Key::F10: ret += "F10"; break;
            case Term::Key::F11: ret += "F11"; break;
            case Term::Key::F12: ret += "F12"; break;
            case Term::Key::F13: ret += "F13"; break;
            case Term::Key::F14: ret += "F14"; break;
            case Term::Key::F15: ret += "F15"; break;
            case Term::Key::F16: ret += "F16"; break;
            case Term::Key::F17: ret += "F17"; break;
            case Term::Key::F18: ret += "F18"; break;
            case Term::Key::F19: ret += "F19"; break;
            case Term::Key::F20: ret += "F20"; break;
            case Term::Key::F21: ret += "F21"; break;
            case Term::Key::F22: ret += "F22"; break;
            case Term::Key::F23: ret += "F23"; break;
            case Term::Key::F24: ret += "F24"; break;
            default:
              if(key.isprint()) ret += static_cast<char>(key);
              else
                ret += "Unknown";
              break;
          }
          Term::terminal << "Key: " << ret << " (" << std::to_string(key) << ") " << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste:
        {
          std::string key_str(event);
          if(!key_str.empty() && key_str[0] == '\033')
          {
            Term::terminal << "You discovered a key combination not yet managed by cpp-terminal (";
            for(std::size_t i = 0; i != key_str.size(); ++i) Term::terminal << static_cast<std::int32_t>(key_str[i]) << " ";
            Term::terminal << ").\nPlease report key combination pressed to " << Term::Homepage << std::endl;
          }
        }
        default: break;
      }
    }
  }
  catch(const Term::Exception& re)
  {
    Term::terminal << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    Term::terminal << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
