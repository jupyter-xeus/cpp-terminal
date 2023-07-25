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
  Event() = default;
  Event(const std::string&);
  Event(const Term::Key&);
  Event(const Term::Screen& screen);
  bool empty() const;
  Type type() const;

  // returns true if the contained type is an empty type
  inline bool is_empty() const { return this->m_Type == Type::Empty; }

  // returns true if the contained type is a key type
  inline bool is_key() const { return this->m_Type == Type::Key; }

  // returns true if the contained type is a screen type
  inline bool is_screen() const { return this->m_Type == Type::Screen; }

  // returns true if the contained type is a cursor type
  inline bool is_cursor() const { return this->m_Type == Type::Cursor; }

  // returns true if the contained type is a copy paset type
  inline bool is_copy_paste() const { return this->m_Type == Type::CopyPaste; }

  // returns a pointer to the contained key type if it is one and otherwise a nullptr
  inline Key* get_if_key() { return (this->is_key()) ? &this->m_Key : nullptr; }

  // returns a const pointer to the contained key type if it is one and otherwise a nullptr
  inline const Key* get_if_key() const { return (this->is_key()) ? &this->m_Key : nullptr; }

  // returns a pointer to the contained screen type if it is one and otherwise a nullptr
  inline Screen* get_if_screen() { return (this->is_screen()) ? &this->m_Screen : nullptr; }

  // returns a const pointer to the contained screen type if it is one and otherwise a nullptr
  inline const Screen* get_if_screen() const { return (this->is_screen()) ? &this->m_Screen : nullptr; }

  // returns a pointer to the contained cursor type if it is one and otherwise a nullptr
  inline Cursor* get_if_cursor() { return (this->is_cursor()) ? &this->m_Cursor : nullptr; }

  // returns a const pointer to the contained cursor type if it is one and otherwise a nullptr
  inline const Cursor* get_if_cursor() const { return (this->is_cursor()) ? &this->m_Cursor : nullptr; }

  // returns a pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  inline std::string* get_if_copy_paste() { return (this->is_copy_paste()) ? &this->m_str : nullptr; }

  // returns a const pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  inline const std::string* get_if_copy_paste() const { return (this->is_copy_paste()) ? &this->m_str : nullptr; }

  operator Term::Key() const;
  operator Term::Screen() const;
  operator Term::Cursor() const;
  operator std::string() const;

private:
  void         parse();
  
#define temp_max_2(a, b)       (((a) > (b)) ? (a) : (b))
#define temp_max_4(a, b, c, d) temp_max_2(temp_max_2(a, b), temp_max_2(c, d))

#define MAX_ALIGN temp_max_4(alignof(std::string), alignof(Term::Key), alignof(Term::Cursor), alignof(Term::Screen))
#define MAX_SIZE  temp_max_4(sizeof(std::string), sizeof(Term::Key), sizeof(Term::Cursor), sizeof(Term::Screen))

  alignas(MAX_ALIGN) uint8_t m_variant[MAX_SIZE];

#undef temp_max_2
#undef temp_max_4
#undef MAX_ALIGN
#undef MAX_SIZE
  
  Type         m_Type{Type::Empty};
  std::string  m_str;
  Key          m_Key{Key::Value::NO_KEY};
  Cursor       m_Cursor;
  Term::Screen m_Screen;
};

}  // namespace Term
