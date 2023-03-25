#pragma once

#include "cpp-terminal/key.hpp"

#include <cstdint>

namespace Term
{

class Event
{
public:
  enum class Type
  {
    Empty,
    Key,
    Char,
  };
  Event() = default;
  Event(const char&);
  Event(const Term::Key&);
  bool empty();
  Type type();
  char getChar() { return m_ch; }
  Key  getKey() { return m_Key; }

private:
  char m_ch;
  Key  m_Key{Key::Value::NO_KEY};
  Type m_Type{Type::Empty};
};

}  // namespace Term
