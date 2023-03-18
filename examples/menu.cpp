#include "cpp-terminal/base.hpp"
#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
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
    Term::Terminal                      term(true, true, true);
    std::pair<std::size_t, std::size_t> term_size = Term::get_size();
    int                                 pos       = 5;
    int                                 h         = 10;
    std::size_t                         w{10};
    bool                                on = true;
    while(on)
    {
      render(std::get<0>(term_size), std::get<1>(term_size), h, w, pos);
      int key = Term::read_key();
      switch(key)
      {
        case Term::Key::ARROW_LEFT:
          if(w > 10) w--;
          break;
        case Term::Key::ARROW_RIGHT:
          if(w < std::get<1>(term_size) - 5) w++;
          break;
        case Term::Key::ARROW_UP:
          if(pos > 1) pos--;
          break;
        case Term::Key::ARROW_DOWN:
          if(pos < h) pos++;
          break;
        case Term::Key::HOME: pos = 1; break;
        case Term::Key::END: pos = h; break;
        case 'q':
        case Term::Key::ESC:
        case Term::Key::CTRL + 'c': on = false; break;
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
