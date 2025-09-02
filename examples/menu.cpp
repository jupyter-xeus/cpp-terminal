/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/terminfo.hpp"
#include "cpp-terminal/tty.hpp"

static void render(const std::size_t& rows, const std::size_t& cols, const std::size_t& menuheight, const std::size_t& menuwidth, const std::size_t& menupos)
{
  static std::string scr;
  scr.reserve(rows * cols * 4);

  scr.append(Term::cursor_move(1, 1));

  const std::size_t menux0{(cols - menuwidth) / 2};
  const std::size_t menuy0{(rows - menuheight) / 2};

  for(std::size_t j = 1; j <= menuy0; ++j) { scr.append("\n"); }

  for(std::size_t j = 1; j <= menux0; ++j) { scr.append(" "); }
  bool hasUTF8{Term::Terminfo::get(Term::Terminfo::Bool::UTF8)};
  if(hasUTF8) { scr.append("┌"); }
  else
  {
    scr.append("+");
  }
  for(std::size_t j = 1; j <= menuwidth; ++j)
  {
    if(hasUTF8) { scr.append("─"); }
    else
    {
      scr.append("-");
    }
  }
  if(hasUTF8) { scr.append("┐"); }
  else
  {
    scr.append("+");
  }
  scr.append(" \n");
  for(std::size_t i = 1; i <= menuheight; ++i)
  {
    for(std::size_t j = 1; j <= menux0; ++j) { scr.append(" "); }
    if(hasUTF8) { scr.append("│"); }
    else
    {
      scr.append("|");
    }
    if(i == menupos)
    {
      scr.append(Term::color_fg(Term::Color::Name::Red));
      scr.append(Term::color_bg(Term::Color::Name::Gray));
      scr.append(Term::style(Term::Style::Bold));
    }
    else
    {
      scr.append(Term::color_fg(Term::Color::Name::Blue));
      scr.append(Term::color_bg(Term::Color::Name::Green));
    }
    const std::string item = std::to_string(i) + ": item";
    scr.append(item);
    for(std::size_t j = 1; j <= (menuwidth - item.size()); ++j) { scr.append(" "); }
    scr.append(Term::color_bg(Term::Color::Name::Default));
    scr.append(Term::color_fg(Term::Color::Name::Default));
    scr.append(Term::style(Term::Style::Reset));
    if(hasUTF8) { scr.append("│"); }
    else
    {
      scr.append("|");
    }
    scr.append(" \n");
  }
  for(std::size_t j = 1; j <= menux0; ++j) { scr.append(" "); }
  if(hasUTF8) { scr.append("└"); }
  else
  {
    scr.append("+");
  }
  for(std::size_t j = 1; j <= menuwidth; ++j)
  {
    if(hasUTF8) { scr.append("─"); }
    else
    {
      scr.append("-");
    }
  }
  if(hasUTF8) { scr.append("┘"); }
  else
  {
    scr.append("+");
  }
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
    Term::terminal.setOptions(Term::Option::Raw, Term::Option::NoSignalKeys, Term::Option::ClearScreen, Term::Option::NoCursor);
    if(!Term::is_stdin_a_tty()) { throw Term::Exception("The terminal is not attached to a TTY and therefore can't catch user input. Exiting..."); }
    Term::Screen term_size = Term::screen_size();
    std::size_t  pos{5};
    std::size_t  h{10};
    std::size_t  w{10};
    bool         on{true};
    bool         need_to_render{true};

    while(on)
    {
      if(need_to_render)
      {
        ::render(term_size.rows(), term_size.columns(), h, w, pos);
        need_to_render = false;
      }
      Term::Event event{Term::read_event()};
      switch(event.type())
      {
        case Term::Event::Type::Key:
        {
          switch(Term::Key(event))
          {
            case Term::Key::ArrowLeft:
              if(w > 10) { --w; }
              need_to_render = true;
              break;
            case Term::Key::ArrowRight:
              if(w < (term_size.columns() - 5)) { ++w; }
              need_to_render = true;
              break;
            case Term::Key::ArrowUp:
              if(pos > 1) { --pos; }
              need_to_render = true;
              break;
            case Term::Key::ArrowDown:
              if(pos < h) { ++pos; }
              need_to_render = true;
              break;
            case Term::Key::Home:
              pos            = 1;
              need_to_render = true;
              break;
            case Term::Key::End:
              pos            = h;
              need_to_render = true;
              break;
            case Term::Key::q:
            case Term::Key::Esc: on = false; break;
            default: break;
          }
          break;
        }
        case Term::Event::Type::Screen:
        {
          term_size = Term::Screen(event);
          Term::cout << Term::clear_screen() << std::flush;
          ::render(term_size.rows(), term_size.columns(), h, w, pos);
          break;
        }
        case Term::Event::Type::Mouse:
        {
          if(Term::Mouse(event).is(Term::Button::Type::Wheel, Term::Button::Action::RolledUp))
          {
            if(pos > 1) { --pos; }
            need_to_render = true;
            break;
          }
          if(Term::Mouse(event).is(Term::Button::Type::Wheel, Term::Button::Action::RolledDown))
          {
            if(pos < h) { ++pos; }
            need_to_render = true;
            break;
          }
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
