/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminal.hpp"

#include <ostream>

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw);
    Term::Cursor cursor{Term::cursor_position()};
    Term::cout << "Cursor position : " << cursor.row() << " " << cursor.column() << std::endl;

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
          if(key == Term::Key::q) { quit++; }
          else
          {
            quit = 0;
            Term::cout << key.name() << std::flush;
            Term::cout << " " << Term::cursor_position_report() << std::flush;
          }
          break;
        }
        case Term::Event::Type::Cursor:
        {
          Term::Cursor cursor(event);
          Term::cout << " [Column(X) : " << cursor.column() << " Row(Y) : " << cursor.row() << "]";
          break;
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
