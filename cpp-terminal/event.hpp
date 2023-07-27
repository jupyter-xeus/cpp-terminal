#pragma once

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/screen.hpp"

#include <cstdint>
#include <string>
#include <iostream>

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
  Event();
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  Event(const Term::Cursor& cursor);
  bool empty() const;
  Type type() const;
  Term::Event& operator=(const Term::Event& event) noexcept;
  Event(const Term::Event& event) noexcept;
  Event( Term::Event && event) noexcept;
  Event& operator=(Event&& other) noexcept;

  operator Term::Key() const;
  operator Term::Screen() const;
  operator Term::Cursor() const;
  operator std::string() const;

private:
  void         parse();
  Type         m_Type;
  union
  {
   Term::Key    m_Key;
   Term::Cursor m_Cursor;
   Term::Screen m_Screen;
  };
  std::string  m_str;
};

}  // namespace Term
