#include <chrono>
#include <cpp-terminal/color.hpp>
#include <cpp-terminal/event.hpp>
#include <cpp-terminal/input.hpp>
#include <cpp-terminal/key.hpp>
#include <cpp-terminal/screen.hpp>
#include <cpp-terminal/terminal.hpp>
#include <thread>
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
        case Term::Event::Type::Focus:
        {
          if(static_cast<Term::Focus>(event).isIn())
          {
            Term::terminal << "Event: Focus In (Cleaning screen !!!)" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            Term::terminal << Term::clear_screen() << std::endl;
            Term::terminal << Term::cursor_move(1, 1) << std::endl;
          }
          else
            Term::terminal << "Event: Focus Out" << std::endl;
          break;
        }
        case Term::Event::Type::Mouse:
        {
          Term::terminal << Term::cursor_move(1, 1) << "CTRL + Q to end" << std::endl;
          Term::Mouse mouse(event);
          std::string button;
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::None)) button += "None ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Left)) button += "Left ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Wheel)) button += "Wheel ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Right)) button += "Right ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button1)) button += "Button1 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button2)) button += "Button2 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button3)) button += "Button3 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button4)) button += "Button4 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button5)) button += "Button5 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button6)) button += "Button6 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button7)) button += "Button7 ";
          if(static_cast<uint32_t>(mouse.getButtons().getType()) & static_cast<std::uint32_t>(Term::Buttons::Type::Button8)) button += "Button8 ";

          std::string action;
          switch(mouse.getButtons().getAction())
          {
            case Term::Buttons::Action::Clicked: action += "Clicked "; break;
            case Term::Buttons::Action::Released: action += "Released "; break;
            case Term::Buttons::Action::DoubleClicked: action += "DoubleClicked "; break;
            case Term::Buttons::Action::RolledUp: action += "RolledUp "; break;
            case Term::Buttons::Action::RolledDown: action += "RolledDown "; break;
            case Term::Buttons::Action::ToRight: action += "ToRight "; break;
            case Term::Buttons::Action::ToLeft: action += "ToLeft "; break;
            case Term::Buttons::Action::None: action += "None "; break;
          }

          if(mouse.getButtons().getAction() == Term::Buttons::Action::DoubleClicked)
          {
            Term::terminal << Term::color_fg(Term::Color::Name::Green) << Term::cursor_move(mouse.row(), mouse.column()) << "*" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::White) << Term::cursor_move(mouse.row(), mouse.column()) << "*" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Green) << Term::cursor_move(mouse.row() - 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Green) << Term::cursor_move(mouse.row() + 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Green) << Term::cursor_move(mouse.row(), mouse.column() + 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Green) << Term::cursor_move(mouse.row(), mouse.column() - 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Term::terminal << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row() - 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row() + 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row(), mouse.column() + 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row(), mouse.column() - 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row() - 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row() + 1, mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row(), mouse.column() + 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row(), mouse.column() - 1) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row() - 1, mouse.column()) << " " << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row() + 1, mouse.column()) << " " << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row(), mouse.column() + 1) << " " << Term::color_fg(Term::Color::Name::Default) << std::endl;
            Term::terminal << Term::color_fg(Term::Color::Name::Red) << Term::cursor_move(mouse.row(), mouse.column() - 1) << " " << Term::color_fg(Term::Color::Name::Default) << std::endl;
          }
          else if(mouse.getButtons().getAction() == Term::Buttons::Action::Clicked)
          {
            Term::terminal << Term::cursor_off() << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row(), mouse.column()) << "@" << Term::color_fg(Term::Color::Name::Default) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Term::terminal << Term::cursor_off() << Term::color_fg(Term::Color::Name::Yellow) << Term::cursor_move(mouse.row(), mouse.column()) << " " << Term::color_fg(Term::Color::Name::Default) << std::endl;
          }
          else if(mouse.getButtons().getAction() == Term::Buttons::Action::None)
          {
            Term::terminal << Term::cursor_off() << Term::cursor_move(mouse.row(), mouse.column()) << "*" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Term::terminal << Term::cursor_off() << Term::cursor_move(mouse.row(), mouse.column()) << " " << std::endl;
          }
          //Term::cursor_move(1,1)<< "Event: Mouse x:"<<mouse.row()<<" y:"<<mouse.column()<<" button : "<<button<<"action(s) : "<<action<< std::endl;
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
