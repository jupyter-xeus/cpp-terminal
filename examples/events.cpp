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
  void print_message()
  {
    while(m_stop == false)
    {
      std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(2000));
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
          if(static_cast<Term::Focus>(event).isIn()) Term::cout << "Event: Focus (In)" << std::endl;
          else
            Term::cout << "Event: Focus (Out)" << std::endl;
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
