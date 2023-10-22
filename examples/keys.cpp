/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
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
    Term::cout << "Cursor position : " << cursor.row() << " " << cursor.column() << std::endl;

    Term::Screen term_size{Term::screen_size()};
    Term::cout << "Dimension:" << term_size.columns() << " " << term_size.rows() << std::endl;
    Term::cout << "Press any key ( 3 time 'q' to quit):" << std::endl;
    int quit{0};
    while(quit != 3)
    {
      Term::Event event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Key:
        {
          Term::Key key(event);
          if(key == Term::Key::q) quit++;
          else
            quit = 0;
          Term::cout << "Key: " << key.name() << " (" << static_cast<std::int32_t>(key) << ")" << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste:
        {
          std::string key_str(event);
          if(!key_str.empty() && key_str[0] == '\033')
          {
            Term::cout << "You discovered a key combination not yet managed by cpp-terminal (";
            for(std::size_t i = 0; i != key_str.size(); ++i) Term::cout << static_cast<std::int32_t>(key_str[i]) << " ";
            Term::cout << ").\nPlease report key combination pressed to " << Term::homepage << std::endl;
          }
        }
        default: break;
      }
    }
  }
  catch(const Term::Exception& re)
  {
    Term::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    Term::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
