#include "cpp-terminal/event.hpp"

bool Term::Event::empty()
{
  if(m_Type == Type::Empty) return true;
  else
    return false;
}

Term::Event::Event(const char& ch) : m_Type(Type::Char), m_ch(ch) {}

Term::Event::Event(const Term::Key& key) : m_Type(Type::Key), m_Key(key) {}

Term::Event::Type Term::Event::type() { return m_Type; }
