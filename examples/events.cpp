#include <cpp-terminal/event.hpp>
#include <cpp-terminal/key.hpp>
#include <cpp-terminal/terminal.hpp>
#include <cpp-terminal/input.hpp>

#include <iostream>

int main()
{
  try
  {
    // set options for the console
    Term::terminal.setOptions(
      Term::Option::ClearScreen, 	// start from a cear screen
      Term::Option::NoSignalKeys, // deactivate key combinations that generate a signal or interrupt
      Term::Option::NoCursor, 	// deactivate the cursor, we will display it ourselfs
      Term::Option::Raw			// get the raw and unprozessed io data from the console buffers
    );

    // initial render of the whole screen
    std::cout << "CTRL + Q to end" << std::endl;

    bool main_loop_continue = true;
    while(main_loop_continue)
    {
      auto event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Empty :
        {
          Term::terminal << "Event: Empty" << std::endl;
          break;
        }
        case Term::Event::Type::Key :
        {
          Term::Key keyEvent = event;
          Term::terminal << "Event: Key ("<<keyEvent.str()<<")" << std::endl;
          if(keyEvent == Term::Key::CTRL_Q) main_loop_continue = false;
          break;
        }
        case Term::Event::Type::Screen :
        {
          Term::terminal << "Event: Screen" << std::endl;
          break;
        }
        case Term::Event::Type::Cursor :
        {
          Term::terminal << "Event: Cursor" << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste :
        {
          Term::terminal << "Event: CopyPaste (" <<static_cast<std::string>(event)<<")"<< std::endl;
          break;
        }
        default :
        {
          Term::terminal << "Event: Unknown" << std::endl;
          break;
        }
      }
    }
  }
  catch(...)
  {
    Term::terminal << "There was an exception!" << std::endl;
  }
  return 0;
}