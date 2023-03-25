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
  bool empty();
  char getChar() { return m_ch; }

private:
  char m_ch;
  Type m_Type{Type::Empty};
};

}  // namespace Term
