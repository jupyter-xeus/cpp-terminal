#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/tty.hpp"
#include "cpp-terminal/window.hpp"

std::string render(Term::Window& scr, const std::size_t& rows, const std::size_t& cols, const std::size_t& menuheight, const std::size_t& menuwidth, const std::size_t& menupos)
{
  scr.clear();
  std::size_t menux0 = (cols - menuwidth) / 2 + 1;
  std::size_t menuy0 = (rows - menuheight) / 2 + 1;
  scr.print_rect(menux0, menuy0, menux0 + menuwidth + 1, menuy0 + menuheight + 1);

  for(std::size_t i = 1; i <= menuheight; i++)
  {
    std::string s = std::to_string(i) + ": item";
    scr.print_str(menux0 + 1, menuy0 + i, s);
    if(i == menupos)
    {
      scr.fill_fg(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i, Term::Color::Name::Red);    // FG
      scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i, Term::Color::Name::Gray);  // BG
      scr.fill_style(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i, Term::Style::BOLD);
    }
    else
    {
      scr.fill_fg(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i, Term::Color::Name::Blue);
      scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i, Term::Color::Name::Green);
    }
  }

  std::size_t y = menuy0 + menuheight + 5;
  scr.print_str(1, y, "Selected item: " + std::to_string(menupos));
  scr.print_str(1, y + 1, "Menu width: " + std::to_string(menuwidth));
  scr.print_str(1, y + 2, "Menu height: " + std::to_string(menuheight));
  scr.print_str(1, y + 3, "Unicode test: Ondřej Čertík, ἐξήκοι");

  return scr.render(1, 1, false);
}

int main()
{
  try
  {
    // check if the terminal is capable of handling input
    if(!Term::is_stdin_a_tty())
    {
      Term::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::NoSignalKeys, Term::Option::NoCursor, Term::Option::Raw);
    Term::Screen term_size = Term::screen_size();
    int          pos       = 5;
    int          h         = 10;
    std::size_t  w{10};
    bool         on = true;
    Term::Window scr(term_size.columns(), term_size.rows());
    Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);
    while(on)
    {
      Term::Event event = Term::read_event();
      switch(event.type())
      {
        case Term::Event::Type::Key:
          Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);
          switch(Term::Key(event))
          {
            case Term::Key::ARROW_LEFT:
              if(w > 10) w--;
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);

              break;
            case Term::Key::ARROW_RIGHT:
              if(w < term_size.columns() - 5) w++;
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);

              break;
            case Term::Key::ARROW_UP:
              if(pos > 1) pos--;
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);

              break;
            case Term::Key::ARROW_DOWN:
              if(pos < h) pos++;
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);

              break;
            case Term::Key::HOME:
              pos = 1;
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);
              break;
            case Term::Key::END:
              Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);

              pos = h;
              break;
            case Term::Key::q:
            case Term::Key::ESC:
            case Term::Key::CTRL_C:
              on = false;
              break;
            default: break;
          }
          break;
        case Term::Event::Type::Screen:
          term_size = Term::Screen(event);
          Term::cout << Term::clear_screen() << std::flush;
          Term::cout << render(scr, term_size.rows(), term_size.columns(), h, w, pos);
          break;
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
