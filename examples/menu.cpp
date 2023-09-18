#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

void render(int rows, int cols, int menuheight, int menuwidth, int menupos)
{
  std::string scr;
  scr.reserve(16 * 1024);

  scr.append(Term::cursor_move(1, 1));

  int menux0 = (cols - menuwidth) / 2;
  int menuy0 = (rows - menuheight) / 2;

  for(int j = 1; j <= menuy0; j++) { scr.append("\n"); }

  for(int j = 1; j <= menux0; j++) { scr.append(" "); }
  if(Term::terminal.supportUTF8()) scr.append("┌");
  else
    scr.append("+");
  for(int j = 1; j <= menuwidth; j++)
  {
    if(Term::terminal.supportUTF8()) scr.append("─");
    else
      scr.append("-");
  }
  if(Term::terminal.supportUTF8()) scr.append("┐");
  else
    scr.append("+");
  scr.append(" \n");
  for(int i = 1; i <= menuheight; i++)
  {
    for(int j = 1; j <= menux0; j++) { scr.append(" "); }
    if(Term::terminal.supportUTF8()) scr.append("│");
    else
      scr.append("|");
    if(i == menupos)
    {
      scr.append(Term::color_fg(Term::Color::Name::Red));
      scr.append(Term::color_bg(Term::Color::Name::Gray));
      scr.append(Term::style(Term::Style::BOLD));
    }
    else
    {
      scr.append(Term::color_fg(Term::Color::Name::Blue));
      scr.append(Term::color_bg(Term::Color::Name::Green));
    }
    std::string s = std::to_string(i) + ": item";
    scr.append(s);
    for(std::size_t j = 1; j <= menuwidth - s.size(); j++) { scr.append(" "); }
    scr.append(Term::color_bg(Term::Color::Name::Default));
    scr.append(Term::color_fg(Term::Color::Name::Default));
    scr.append(Term::style(Term::Style::RESET));
    if(Term::terminal.supportUTF8()) scr.append("│");
    else
      scr.append("|");
    scr.append(" \n");
  }
  for(int j = 1; j <= menux0; j++) { scr.append(" "); }
  if(Term::terminal.supportUTF8()) scr.append("└");
  else
    scr.append("+");
  for(int j = 1; j <= menuwidth; j++)
  {
    if(Term::terminal.supportUTF8()) scr.append("─");
    else
      scr.append("-");
  }
  if(Term::terminal.supportUTF8()) scr.append("┘");
  else
    scr.append("+");
  scr.append(" \n");

  scr.append(Term::cursor_move(menuy0 + menuheight + 5, 1));
  scr.append("Selected item: " + std::to_string(menupos) + "      \n");
  scr.append("Menu width: " + std::to_string(menuwidth) + "       \n");
  scr.append("Menu height: " + std::to_string(menuheight) + "    \n");

  Term::cout << scr << std::flush;
}

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw);
    if(!Term::is_stdin_a_tty()) throw Term::Exception("The terminal is not attached to a TTY and therefore can't catch user input. Exiting...");
    Term::Screen term_size = Term::screen_size();
    int          pos       = 5;
    int          h         = 10;
    std::size_t  w{10};
    bool         on = true;
    while(on)
    {
      Term::Event event{Term::read_event()};
      switch(event.type())
      {
        case Term::Event::Type::Key:
        {
          switch(Term::Key(event))
          {
            case Term::Key::ArrowLeft:
              if(w > 10) w--;
              break;
            case Term::Key::ArrowRight:
              if(w < (term_size.columns() - 5)) w++;
              break;
            case Term::Key::ArrowUp:
              if(pos > 1) pos--;
              break;
            case Term::Key::ArrowDown:
              if(pos < h) pos++;
              break;
            case Term::Key::Home: pos = 1; break;
            case Term::Key::End: pos = h; break;
            case Term::Key::q:
            case Term::Key::Esc:
            case Term::Key::Ctrl_C: on = false;
            default: break;
          }
          render(term_size.rows(), term_size.columns(), h, w, pos);
          break;
        }
        case Term::Event::Type::Screen:
        {
          term_size = Term::Screen(event);
          Term::cout << Term::clear_screen() << std::flush;
          render(term_size.rows(), term_size.columns(), h, w, pos);
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
