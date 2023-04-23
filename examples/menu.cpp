#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>

void render(int rows, int cols, int menuheight, int menuwidth, int menupos)
{
  std::cout << Term::clear_screen() << std::flush;
  std::string scr;
  scr.reserve(16 * 1024);

  scr.append(Term::cursor_move(1, 1));

  int menux0 = (cols - menuwidth) / 2;
  int menuy0 = (rows - menuheight) / 2;

  for(int j = 1; j <= menuy0; j++) { scr.append("\n"); }

  for(int j = 1; j <= menux0; j++) { scr.append(" "); }
  scr.append("┌");
  for(int j = 1; j <= menuwidth; j++) { scr.append("─"); }
  scr.append("┐");
  scr.append(" \n");
  for(int i = 1; i <= menuheight; i++)
  {
    for(int j = 1; j <= menux0; j++) { scr.append(" "); }
    scr.append("│");
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
    scr.append("│");
    scr.append(" \n");
  }
  for(int j = 1; j <= menux0; j++) { scr.append(" "); }
  scr.append("└");
  for(int j = 1; j <= menuwidth; j++) { scr.append("─"); }
  scr.append("┘");
  scr.append(" \n");

  scr.append(Term::cursor_move(menuy0 + menuheight + 5, 1));
  scr.append("Selected item: " + std::to_string(menupos) + "      \n");
  scr.append("Menu width: " + std::to_string(menuwidth) + "       \n");
  scr.append("Menu height: " + std::to_string(menuheight) + "    \n");

  std::cout << scr << std::flush;
}

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    if(!Term::is_stdin_a_tty())
    {
      std::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Term::terminal.setOptions({Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw});
    Term::Screen term_size = Term::screen_size();
    int          pos       = 5;
    int          h         = 10;
    std::size_t  w{10};
    bool         on = true;
    while(on)
    {
      render(term_size.rows(), term_size.columns(), h, w, pos);
      Term::Event event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Key:
          switch(Term::Key(event))
          {
            case Term::Key::ARROW_LEFT:
              if(w > 10) w--;
              break;
            case Term::Key::ARROW_RIGHT:
              if(w < (term_size.columns() - 5)) w++;
              break;
            case Term::Key::ARROW_UP:
              if(pos > 1) pos--;
              break;
            case Term::Key::ARROW_DOWN:
              if(pos < h) pos++;
              break;
            case Term::Key::HOME: pos = 1; break;
            case Term::Key::END: pos = h; break;
            case Term::Key::q:
            case Term::Key::ESC:
            case Term::Key::CTRL_C: on = false; break;
            default: break;
          }
          break;
        case Term::Event::Type::Screen:
          term_size = Term::Screen(event);
          std::cout << Term::clear_screen() << std::flush;
          render(term_size.rows(), term_size.columns(), h, w, pos);
          break;
        default: break;
      }
    }
  }
  catch(const Term::Exception& re)
  {
    std::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
