// Bug in some GCC
#if !defined(_GLIBCXX_USE_NANOSLEEP)
  #define _GLIBCXX_USE_NANOSLEEP
#endif

#include "cpp-terminal/input.hpp"

#include "cpp-terminal/event.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>

std::int32_t Term::read_key()
{
  std::int32_t key{Key::NO_KEY};
  while((key = read_key0()) == Key::NO_KEY) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
  return key;
}

std::int32_t Term::read_key0()
{
  Term::Event event = Platform::read_raw();
  if(event.empty()) return Key::NO_KEY;
  Term::Key key = Key(static_cast<Term::Key::Value>(event.getChar()));

  if(key.is_CTRL()) { return key; }
  else if(key == Key::ESC)
  {
    Term::Event seq[4]{'\0', '\0', '\0', '\0'};

    if((seq[0] = Platform::read_raw()).empty()) return Key::ESC;
    if((seq[1] = Platform::read_raw()).empty())
    {
      if(seq[0].getChar() >= Term::Key::Value::a && seq[0].getChar() <= Term::Key::Value::z)
      {
        // gnome-term, Windows Console
        return Key::ALT + seq[0].getChar();
      }
      if(seq[0].getChar() == '\x0d')
      {
        // gnome-term
        return Key::ALT_ENTER;
      }
      return -1;
    }

    if(seq[0].getChar() == '[')
    {
      if(seq[1].getChar() >= '0' && seq[1].getChar() <= '9')
      {
        if((seq[2] = Platform::read_raw()).empty()) { return -2; }
        if(seq[2].getChar() == '~')
        {
          switch(seq[1].getChar())
          {
            case '1': return Key::HOME;
            case '2': return Key::INSERT;
            case '3': return Key::Delete;
            case '4': return Key::END;
            case '5': return Key::PAGE_UP;
            case '6': return Key::PAGE_DOWN;
            case '7': return Key::HOME;
            case '8': return Key::END;
          }
        }
        else if(seq[2].getChar() == ';')
        {
          if(seq[1].getChar() == '1')
          {
            if((seq[2] = Platform::read_raw()).empty()) { return -10; }
            if((seq[3] = Platform::read_raw()).empty()) { return -11; }
            if(seq[2].getChar() == '5')
            {
              switch(seq[3].getChar())
              {
                case 'A': return Key::CTRL_UP;
                case 'B': return Key::CTRL_DOWN;
                case 'C': return Key::CTRL_RIGHT;
                case 'D': return Key::CTRL_LEFT;
              }
            }
            return -12;
          }
        }
        else
        {
          if(seq[2].getChar() >= '0' && seq[2].getChar() <= '9')
          {
            if((seq[3] = Platform::read_raw()).empty()) { return -3; }
            if(seq[3].getChar() == '~')
            {
              if(seq[1].getChar() == '1')
              {
                switch(seq[2].getChar())
                {
                  case '5': return Key::F5;
                  case '7': return Key::F6;
                  case '8': return Key::F7;
                  case '9': return Key::F8;
                }
              }
              else if(seq[1].getChar() == '2')
              {
                switch(seq[2].getChar())
                {
                  case '0': return Key::F9;
                  case '1': return Key::F10;
                  case '3': return Key::F11;
                  case '4': return Key::F12;
                }
              }
            }
          }
        }
      }
      else
      {
        switch(seq[1].getChar())
        {
          case 'A': return Key::ARROW_UP;
          case 'B': return Key::ARROW_DOWN;
          case 'C': return Key::ARROW_RIGHT;
          case 'D': return Key::ARROW_LEFT;
          case 'E': return Key::NUMERIC_5;
          case 'H': return Key::HOME;
          case 'F': return Key::END;
        }
      }
    }
    else if(seq[0].getChar() == 'O')
    {
      switch(seq[1].getChar())
      {
        case 'F': return Key::END;
        case 'H': return Key::HOME;
        case 'P': return Key::F1;
        case 'Q': return Key::F2;
        case 'R': return Key::F3;
        case 'S': return Key::F4;
      }
    }
    return -4;
  }
  else
  {
    switch(key)
    {
      case Key::Delete: return Key::BACKSPACE;
      case Key::LF:
      case Key::CR: return Key::ENTER;
    }
    if(key == '\xc3')
    {
      if((event = Platform::read_raw()).empty()) { return -8; }
      else
      {
        if(event.getChar() >= '\xa1' && event.getChar() <= '\xba')
        {
          // xterm
          return Key::ALT + (event.getChar() + 'a' - '\xa1');
        }
        return -9;
      }
    }
    else if(event.getChar() == '\xc2')
    {
      if((event = Platform::read_raw()).empty()) { return -10; }
      else
      {
        if(event.getChar() == '\x8d')
        {
          // xterm
          return Key::ALT_ENTER;
        }
        return -11;
      }
    }
    return event.getChar();
  }
}

// returns the whole input from STDIN as string
std::string Term::read_stdin()
{
  std::string file;
  char        c{'\0'};
  while(true)
  {
    c = Platform::read_raw_stdin();
    if(c == 0x04) return file;
    else { file.push_back(c); }
  }
}
