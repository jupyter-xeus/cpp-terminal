#pragma once

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
    CopyPaste,
  };
  Event() = default;
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  bool empty();
  Type type();

      operator Term::Key();
          operator Term::Screen();
  Key getKey() { return m_Key; }

private:
  void        parse();
  Type        m_Type{Type::Empty};
  std::string m_str;
  Key         m_Key{Key::Value::NO_KEY};
  Term::Screen m_Screen;
};

}  // namespace Term
