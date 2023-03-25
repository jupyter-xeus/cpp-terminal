#include "cpp-terminal/event.hpp"

bool Term::Event::empty()
{
  if(m_Type == Type::Empty) return true;
  else
    return false;
}

Term::Event::Event(const char& ch) : m_Type(Type::Char), m_ch(ch) {}
