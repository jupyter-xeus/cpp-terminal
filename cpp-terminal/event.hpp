#pragma once

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/focus.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/mouse.hpp"
#include "cpp-terminal/screen.hpp"

#include <cstdint>
#include <string>

namespace Term
{

class Event
{
public:
  enum class Type
  {
    Empty,
    Key,
    Screen,
    Cursor,
    CopyPaste,
    Focus,
    Mouse,
  };
  Event() = default;
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  Event(const Term::Focus& focus);
  Event(const Term::Mouse& mouse);
  bool empty();
  Type type();

  operator Term::Key();
  operator Term::Screen();
  operator Term::Cursor();
  operator Term::Focus();
  operator std::string();
  operator Term::Mouse();

private:
  void         parse();
  Type         m_Type{Type::Empty};
  std::string  m_str;
  Term::Key    m_Key{Key::Value::NO_KEY};
  Term::Cursor m_Cursor;
  Term::Screen m_Screen;
  Term::Focus  m_Focus;
  Term::Mouse  m_Mouse;
};

}  // namespace Term
