#pragma once

#include "cpp-terminal/key.hpp"

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
    CopyPaste,
  };
  Event() = default;
  Event(const std::string&);
  Event(const Term::Key&);
  bool empty();
  Type type();

  operator Term::Key();
  Key  getKey() { return m_Key; }

private:
  void parse();
  std::string m_str;
  Key  m_Key{Key::Value::NO_KEY};
  Type m_Type{Type::Empty};
};

}  // namespace Term
