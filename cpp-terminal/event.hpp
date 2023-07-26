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

  // default constructs an event. A default constructed event is an empty event.
  Event() = default;

  // copy constructs an event
  Event(const Event& event);

  // move constructs an event, which makes the other event an empty event
  Event(Event&& event);

  // construcst a key event
  Event(const Term::Key& key);

  // constructs a screen event
  Event(const Term::Screen& screen);

  // constructs a cursor event
  Event(const Term::Cursor& cursor);

  // constructs a copy paste event by copying the string
  Event(const std::string& str);

  // constructs a copy paste event by moveing the string
  Event(std::string&& str);

  // safely deconstrucst an event by calling the destructor of the stored type
  ~Event();

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal
  Event& operator=(const Event& event);

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
  Event& operator=(Event&& event);

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal
  Event& assign(const Event& event);

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
  Event& assign(Event&& event);

  // copy assigns a key event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  Event& assign(const Term::Key& key);

  // copy assigns a screen event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  Event& assign(const Term::Screen& screen);

  // copy assigns a cursor event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  Event& assign(const Term::Cursor& cursor);

  // copy assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  Event& assign(const std::string& str);

  // move assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  Event& assign(std::string&& str);

  // destructs the containing type and makes this an empty type
  Event& clear();

  // returns true if this is an empty type
  bool empty() const;

  // returns the contained type enum
  Type type() const;

  // returns true if the contained type is an empty type
  bool is_empty() const;

  // returns true if the contained type is a key type
  bool is_key() const;

  // returns true if the contained type is a screen type
  bool is_screen() const;

  // returns true if the contained type is a cursor type
  bool is_cursor() const;

  // returns true if the contained type is a copy paset type
  bool is_copy_paste() const;

  // returns a pointer to the contained key type if it is one and otherwise a nullptr
  Key* get_if_key();

  // returns a const pointer to the contained key type if it is one and otherwise a nullptr
  const Key* get_if_key() const;

  // returns a pointer to the contained screen type if it is one and otherwise a nullptr
  Screen* get_if_screen();

  // returns a const pointer to the contained screen type if it is one and otherwise a nullptr
  const Screen* get_if_screen() const;

  // returns a pointer to the contained cursor type if it is one and otherwise a nullptr
  Cursor* get_if_cursor();

  // returns a const pointer to the contained cursor type if it is one and otherwise a nullptr
  const Cursor* get_if_cursor() const;

  // returns a pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  std::string* get_if_copy_paste();

  // returns a const pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  const std::string* get_if_copy_paste() const;

  // constructs a Key from the contained Key if it is one and otherwise returns a default one
  operator Term::Key() const;

  // constructs a Screen from the contained Screen if it is one and otherwise returns a default one
  operator Term::Screen() const;

  // constructs a Cursor from the contained Cursor if it is one and otherwise returns a default one
  operator Term::Cursor() const;

  // constructs a std::string from the contained Copy Paste Event if it is one and otherwise returns a default one
  operator std::string() const;

private:
#define term_temp_max_2(a, b)       (((a) > (b)) ? (a) : (b))
#define term_temp_max_4(a, b, c, d) term_temp_max_2(term_temp_max_2(a, b), term_temp_max_2(c, d))

#define TERM_NEEDED_SIZE term_temp_max_4(sizeof(std::string), sizeof(Key), sizeof(Cursor), sizeof(Term::Screen))
#define TERM_ACTUAL_SIZE (TERM_NEEDED_SIZE + sizeof(void*) - 1) / sizeof(void*)

  void* m_variant[TERM_ACTUAL_SIZE];

#undef term_temp_max_2
#undef term_temp_max_4
#undef TERM_NEEDED_SIZE
#undef TERM_ACTUAL_SIZE

  Type m_Type{Type::Empty};
};

// construcst an event by parsing a string in ANSI format
// returns an event from the string
Event parse_event(std::string&& str);

}  // namespace Term
