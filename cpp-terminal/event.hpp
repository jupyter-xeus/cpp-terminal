#pragma once

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/focus.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/screen.hpp"

#include <cstdint>
#include <memory>
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
    Focus,
    CopyPaste,
  };
  Event();
  ~Event();
  Event(const std::string& str);
  Event(const Term::Key& key);
  Event(const Term::Screen& screen);
  Event(const Term::Cursor& cursor);
  Event(const Term::Focus& focus);
  bool         empty() const;
  Type         type() const;
  Term::Event& operator=(const Term::Event& event);
  Event(const Term::Event& event);
  Event(Term::Event&& event) noexcept;
  Event& operator=(Event&& other) noexcept;

  operator Term::Key() const;
  operator Term::Screen() const;
  operator Term::Cursor() const;
  operator Term::Focus() const;
  operator std::string() const;

  // getters
  Key*              get_if_key();
  const Key*        get_if_key() const;
  Screen*           get_if_screen();
  const Screen*     get_if_screen() const;
  Cursor*           get_if_cursor();
  const Cursor*     get_if_cursor() const;
  Focus*            get_if_focus();
  const Focus*      get_if_focus() const;
  std::string       get_if_copy_paste();
  const std::string get_if_copy_paste() const;

private:
  void parse(const std::string&);
  union container
  {
    container();
    ~container();
    container(const container&)                         = delete;
    container(container&&)                              = delete;
    container&              operator=(const container&) = delete;
    container&              operator=(container&&)      = delete;
    Term::Key               m_Key;
    Term::Cursor            m_Cursor;
    Term::Screen            m_Screen;
    Term::Focus             m_Focus;
    std::unique_ptr<char[]> m_string{nullptr};
  };
  Type      m_Type{Type::Empty};
  container m_container;
};

}  // namespace Term
