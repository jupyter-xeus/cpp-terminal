/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#if !defined(_GLIBCXX_USE_NANOSLEEP)
  #define _GLIBCXX_USE_NANOSLEEP
#endif

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"

#include <chrono>
#include <cpp-terminal/event.hpp>
#include <cpp-terminal/input.hpp>
#include <cpp-terminal/key.hpp>
#include <thread>

// Same than event but add 1s to slow down the event processing.
int main()
{
  try
  {
    // set options for the console
    Term::terminal.setOptions(Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw);
    // initial render of the whole screen
    Term::cout << "CTRL + Q to end" << std::endl;
    bool main_loop_continue = true;
    while(main_loop_continue)
    {
      auto event = Term::read_event();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      switch(event.type())
      {
        case Term::Event::Type::Empty:
        {
          Term::cout << "Event: Empty" << std::endl;
          break;
        }
        case Term::Event::Type::Key:
        {
          Term::Key keyEvent = event;
          if(keyEvent.iscntrl()) { Term::cout << "Event: Key (control key)" << std::endl; }
          else { Term::cout << "Event: Key (" << keyEvent.str() << ")" << std::endl; }
          if(keyEvent == Term::Key::Ctrl_Q) { main_loop_continue = false; }
          break;
        }
        case Term::Event::Type::Screen:
        {
          Term::Screen screen(event);
          Term::cout << "Event: Screen (" << screen.rows() << "," << screen.columns() << ")" << std::endl;
          break;
        }
        case Term::Event::Type::Cursor:
        {
          Term::cout << "Event: Cursor" << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste:
        {
          Term::cout << "Event: CopyPaste (" << static_cast<std::string>(event) << ")" << std::endl;
          break;
        }
        default:
        {
          Term::cout << "Event: Unknown" << std::endl;
          break;
        }
      }
    }
  }
  catch(...)
  {
    Term::cerr << "There was an exception!" << std::endl;
  }
  return 0;
}
