/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"

#include <cpp-terminal/input.hpp>
#include <cpp-terminal/key.hpp>
#include <iostream>
#if !defined(_GLIBCXX_USE_NANOSLEEP)  // g++ bug
  #define _GLIBCXX_USE_NANOSLEEP
#endif
#include <chrono>
#include <thread>

class Loop
{
public:
  void stop() { m_stop = true; }
  void print_message() const
  {
    while(!m_stop)
    {
      std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(200));
      std::cout << "\tNot waiting..." << std::endl;
    }
  }

private:
  bool m_stop{false};
};

int main()
{
  try
  {
    // set options for the console
    Term::terminal.setOptions(Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw);
    // initial render of the whole screen
    Term::cout << "CTRL + Q to end" << std::endl;
    bool        main_loop_continue = true;
    Loop        loop;
    std::thread looper(&Loop::print_message, &loop);  // Just to check this is never stopped.
    while(main_loop_continue)
    {
      auto event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Empty:
        {
          Term::cout << "Event: Empty" << std::endl;
          break;
        }
        case Term::Event::Type::Focus:
        {
          if(static_cast<Term::Focus>(event).in()) { Term::cout << "Event: Focus (In)" << std::endl; }
          else { Term::cout << "Event: Focus (Out)" << std::endl; }
          break;
        }
        case Term::Event::Type::Key:
        {
          Term::Key keyEvent = event;
          Term::cout << "Event: Key (" << keyEvent.name() << ")" << std::endl;
          if(keyEvent == Term::Key::Ctrl_Q)
          {
            main_loop_continue = false;
            loop.stop();
          }
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
        case Term::Event::Type::Mouse:
        {
          Term::Mouse mouse(event);
          std::string buttons;
          switch(mouse.getButton().type())
          {
            case Term::Button::Type::Left:
            {
              buttons += "Type (left) ";
              break;
            }
            case Term::Button::Type::Wheel:
            {
              buttons += "Type (wheel) ";
              break;
            }
            case Term::Button::Type::Right:
            {
              buttons += "Type (right) ";
              break;
            }
            case Term::Button::Type::Button1:
            {
              buttons += "Type (button1) ";
              break;
            }
            case Term::Button::Type::Button2:
            {
              buttons += "Type (button2) ";
              break;
            }
            case Term::Button::Type::Button3:
            {
              buttons += "Type (button3) ";
              break;
            }
            case Term::Button::Type::Button4:
            {
              buttons += "Type (button4) ";
              break;
            }
            case Term::Button::Type::Button5:
            {
              buttons += "Type (button5) ";
              break;
            }
            case Term::Button::Type::Button6:
            {
              buttons += "Type (button6) ";
              break;
            }
            case Term::Button::Type::Button7:
            {
              buttons += "Type (button7) ";
              break;
            }
            case Term::Button::Type::Button8:
            {
              buttons += "Type (button8) ";
              break;
            }
            default: break;
          }
          switch(mouse.getButton().action())
          {
            case Term::Button::Action::ToLeft:
            {
              buttons += "Action (to left) ";
              break;
            }
            case Term::Button::Action::ToRight:
            {
              buttons += "Action (to right) ";
              break;
            }
            case Term::Button::Action::RolledUp:
            {
              buttons += "Action (rolled up) ";
              break;
            }
            case Term::Button::Action::RolledDown:
            {
              buttons += "Action (rolled down) ";
              break;
            }
            case Term::Button::Action::DoubleClicked:
            {
              buttons += "Action (double clicked) ";
              break;
            }
            case Term::Button::Action::Pressed:
            {
              buttons += "Action (pressed) ";
              break;
            }
            case Term::Button::Action::Released:
            {
              buttons += "Action (released) ";
              break;
            }
            default: break;
          }
          Term::cout << "Event: Mouse Position(" << mouse.row() << ";" << mouse.column() << ") " << buttons << std::endl;
          break;
        }
        default:
        {
          Term::cout << "Event: Unknown" << std::endl;
          break;
        }
      }
    }
    looper.join();
  }
  catch(...)
  {
    Term::cerr << "There was an exception!" << std::endl;
  }
}
