#pragma once

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/key.hpp"
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
  };
  Event() = default;
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  bool empty() const;
  Type type() const;

  operator Term::Key() const;
  operator Term::Screen() const;
  operator Term::Cursor() const;
  operator std::string() const;

private:
  void         parse();
  Type         m_Type{Type::Empty};
  std::string  m_str;
  Key          m_Key{Key::Value::NO_KEY};
  Cursor       m_Cursor;
  Term::Screen m_Screen;
};

}  // namespace Term
