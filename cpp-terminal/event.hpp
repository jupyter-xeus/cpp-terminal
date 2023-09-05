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
  Event();
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  Event(const Term::Cursor& cursor);
  bool         empty() const;
  Type         type() const;
  Term::Event& operator=(const Term::Event& event);
  Event(const Term::Event& event);
  Event(Term::Event&& event) noexcept;
  Event& operator=(Event&& other) noexcept;
  ~Event();

  operator Term::Key() const;
  operator Term::Screen() const;
  operator Term::Cursor() const;
  operator std::string() const;

  // getters
  Key*               get_if_key();
  const Key*         get_if_key() const;
  Screen*            get_if_screen();
  const Screen*      get_if_screen() const;
  Cursor*            get_if_cursor();
  const Cursor*      get_if_cursor() const;
  std::string*       get_if_copy_paste();
  const std::string* get_if_copy_paste() const;

private:
  void parse(const std::string&);
  Type m_Type{Type::Empty};
  union container
  {
    container();
    Term::Key    m_Key;
    Term::Cursor m_Cursor;
    Term::Screen m_Screen;
  };
  container   m_container;
  std::string m_str;
};

}  // namespace Term
