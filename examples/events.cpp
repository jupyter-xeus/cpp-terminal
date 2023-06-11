#include <cpp-terminal/event.hpp>
#include <cpp-terminal/input.hpp>
#include <cpp-terminal/key.hpp>
#include <cpp-terminal/terminal.hpp>

int main()
{
  try
  {
    // set options for the console
    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw);
    // initial render of the whole screen
    Term::terminal << "CTRL + Q to end" << std::endl;
    bool main_loop_continue = true;
    while(main_loop_continue)
    {
      auto event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Empty:
        {
          Term::terminal << "Event: Empty" << std::endl;
          break;
        }
        case Term::Event::Type::Key:
        {
          Term::Key keyEvent = event;
          if(keyEvent.iscntrl()) Term::terminal << "Event: Key (control key)" << std::endl;
          else
            Term::terminal << "Event: Key (" << keyEvent.str() << ")" << std::endl;
          if(keyEvent == Term::Key::CTRL_Q) main_loop_continue = false;
          break;
        }
        case Term::Event::Type::Screen:
        {
          Term::terminal << "Event: Screen" << std::endl;
          break;
        }
        case Term::Event::Type::Cursor:
        {
          Term::terminal << "Event: Cursor" << std::endl;
          break;
        }
        case Term::Event::Type::CopyPaste:
        {
          Term::terminal << "Event: CopyPaste (" << static_cast<std::string>(event) << ")" << std::endl;
          break;
        }
        default:
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
