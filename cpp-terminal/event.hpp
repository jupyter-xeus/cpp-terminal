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
  inline Event(const Event& event) : m_Type(event.m_Type)
  {
    switch(event.m_Type)
    {
      case Type::Empty: break;
      case Type::Key: new(reinterpret_cast<void*>(this->m_variant)) Term::Key(*reinterpret_cast<const Key*>(event.m_variant)); break;
      case Type::Screen: new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(*reinterpret_cast<const Screen*>(event.m_variant)); break;
      case Type::Cursor: new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(*reinterpret_cast<const Cursor*>(event.m_variant)); break;
      case Type::CopyPaste: new(reinterpret_cast<void*>(this->m_variant)) std::string(*reinterpret_cast<const std::string*>(event.m_variant)); break;
    }
  }

  // move constructs an event, which makes the other event an empty event
  inline Event(Event&& event) : m_Type(event.m_Type)
  {
    switch(event.m_Type)
    {
      case Type::Empty: break;
      case Type::Key: new(reinterpret_cast<void*>(this->m_variant)) Term::Key(std::move(*reinterpret_cast<Key*>(event.m_variant))); break;
      case Type::Screen: new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(std::move(*reinterpret_cast<Screen*>(event.m_variant))); break;
      case Type::Cursor: new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(std::move(*reinterpret_cast<Cursor*>(event.m_variant))); break;
      case Type::CopyPaste: new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(*reinterpret_cast<std::string*>(event.m_variant))); break;
    }
    event.m_Type = Type::Empty;
  }

  // construcst a key event
  inline Event(const Term::Key& key) : m_Type(Type::Key) { new(reinterpret_cast<void*>(this->m_variant)) Term::Key(key); }

  // constructs a screen event
  inline Event(const Term::Screen& screen) : m_Type(Type::Screen) { new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(screen); }

  // constructs a cursor event
  inline Event(const Term::Cursor& cursor) : m_Type(Type::Cursor) { new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(cursor); }

  // constructs a copy paste event by copying the string
  inline Event(const std::string& str) : m_Type(Type::CopyPaste) { new(reinterpret_cast<void*>(this->m_variant)) std::string(str); }

  // constructs a copy paste event by moveing the string
  inline Event(std::string&& str) : m_Type(Type::CopyPaste) { new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(str)); }

  // construcst an event by parsing a string in ANSI format
  static Event parse(const std::string& str);

  // safely deconstrucst an event by calling the destructor of the stored type
  inline ~Event() { this->clear(); }

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal
  inline Event& operator=(const Event& event) { return this->assign(event); }

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
  inline Event& operator=(Event&& event) { return this->assign(std::move(event)); }

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal
  inline Event& assign(const Event& event)
  {
    switch(event.m_Type)
    {
      case Type::Empty: this->clear(); break;
      case Type::Key: this->assign(*reinterpret_cast<const Key*>(event.m_variant)); break;
      case Type::Screen: this->assign(*reinterpret_cast<const Screen*>(event.m_variant)); break;
      case Type::Cursor: this->assign(*reinterpret_cast<const Cursor*>(event.m_variant)); break;
      case Type::CopyPaste: this->assign(*reinterpret_cast<const std::string*>(event.m_variant)); break;
    }
    return *this;
  }

  // assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
  inline Event& assign(Event&& event)
  {
    switch(event.m_Type)
    {
      case Type::Empty: this->clear(); break;
      case Type::Key: this->assign(std::move(*reinterpret_cast<Key*>(event.m_variant))); break;
      case Type::Screen: this->assign(std::move(*reinterpret_cast<Screen*>(event.m_variant))); break;
      case Type::Cursor: this->assign(std::move(*reinterpret_cast<Cursor*>(event.m_variant))); break;
      case Type::CopyPaste: this->assign(std::move(*reinterpret_cast<std::string*>(event.m_variant))); break;
    }
    event.m_Type = Type::Empty;
    return *this;
  }

  // copy assigns a key event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  inline Event& assign(const Term::Key& key)
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: *reinterpret_cast<Key*>(this->m_variant) = key; return *this;
      case Type::Screen: reinterpret_cast<Screen*>(this->m_variant)->~Screen(); break;
      case Type::Cursor: reinterpret_cast<Cursor*>(this->m_variant)->~Cursor(); break;
      case Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
    }
    new(reinterpret_cast<void*>(this->m_variant)) Key(key);
    this->m_Type = Type::Key;
    return *this;
  }

  // copy assigns a screen event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  inline Event& assign(const Term::Screen& screen)
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: reinterpret_cast<Key*>(this->m_variant)->~Key(); break;
      case Type::Screen:
        *reinterpret_cast<Screen*>(this->m_variant) = screen;
        return *this;
        break;
      case Type::Cursor: reinterpret_cast<Cursor*>(this->m_variant)->~Cursor(); break;
      case Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
    }
    new(reinterpret_cast<void*>(this->m_variant)) Screen(screen);
    this->m_Type = Type::Screen;
    return *this;
  }

  // copy assigns a cursor event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  inline Event& assign(const Term::Cursor& cursor)
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: reinterpret_cast<Key*>(this->m_variant)->~Key(); break;
      case Type::Screen: reinterpret_cast<Screen*>(this->m_variant)->~Screen(); break;
      case Type::Cursor:
        *reinterpret_cast<Cursor*>(this->m_variant) = cursor;
        return *this;
        break;
      case Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
    }
    new(reinterpret_cast<void*>(this->m_variant)) Cursor(cursor);
    this->m_Type = Type::Cursor;
    return *this;
  }

  // copy assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  inline Event& assign(const std::string& str)
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: reinterpret_cast<Key*>(this->m_variant)->~Key(); break;
      case Type::Screen: reinterpret_cast<Screen*>(this->m_variant)->~Screen(); break;
      case Type::Cursor: reinterpret_cast<Cursor*>(this->m_variant)->~Cursor(); break;
      case Type::CopyPaste:
        reinterpret_cast<std::string*>(this->m_variant)->assign(str);
        return *this;
        break;
    }
    new(reinterpret_cast<void*>(this->m_variant)) std::string(str);
    this->m_Type = Type::CopyPaste;
    return *this;
  }

  // move assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
  inline Event& assign(std::string&& str)
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: reinterpret_cast<Key*>(this->m_variant)->~Key(); break;
      case Type::Screen: reinterpret_cast<Screen*>(this->m_variant)->~Screen(); break;
      case Type::Cursor: reinterpret_cast<Cursor*>(this->m_variant)->~Cursor(); break;
      case Type::CopyPaste:
        reinterpret_cast<std::string*>(this->m_variant)->assign(std::move(str));
        return *this;
        break;
    }
    new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(str));
    this->m_Type = Type::CopyPaste;
    return *this;
  }

  // destructs the containing type and makes this an empty type
  inline Event& clear()
  {
    switch(this->m_Type)
    {
      case Type::Empty: break;
      case Type::Key: reinterpret_cast<Key*>(this->m_variant)->~Key(); break;
      case Type::Screen: reinterpret_cast<Screen*>(this->m_variant)->~Screen(); break;
      case Type::Cursor: reinterpret_cast<Cursor*>(this->m_variant)->~Cursor(); break;
      case Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
    }
    this->m_Type = Type::Empty;
    return *this;
  }

  // returns true if this is an empty type
  inline bool empty() const { return this->m_Type == Type::Empty; }

  // returns the contained type enum
  inline Type type() const { return this->m_Type; };

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
  inline Key* get_if_key() { return (this->is_key()) ? reinterpret_cast<Key*>(this->m_variant) : nullptr; }

  // returns a const pointer to the contained key type if it is one and otherwise a nullptr
  inline const Key* get_if_key() const { return (this->is_key()) ? reinterpret_cast<const Key*>(this->m_variant) : nullptr; }

  // returns a pointer to the contained screen type if it is one and otherwise a nullptr
  inline Screen* get_if_screen() { return (this->is_screen()) ? reinterpret_cast<Screen*>(this->m_variant) : nullptr; }

  // returns a const pointer to the contained screen type if it is one and otherwise a nullptr
  inline const Screen* get_if_screen() const { return (this->is_screen()) ? reinterpret_cast<const Screen*>(this->m_variant) : nullptr; }

  // returns a pointer to the contained cursor type if it is one and otherwise a nullptr
  inline Cursor* get_if_cursor() { return (this->is_cursor()) ? reinterpret_cast<Cursor*>(this->m_variant) : nullptr; }

  // returns a const pointer to the contained cursor type if it is one and otherwise a nullptr
  inline const Cursor* get_if_cursor() const { return (this->is_cursor()) ? reinterpret_cast<const Cursor*>(this->m_variant) : nullptr; }

  // returns a pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  inline std::string* get_if_copy_paste() { return (this->is_copy_paste()) ? reinterpret_cast<std::string*>(this->m_variant) : nullptr; }

  // returns a const pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
  inline const std::string* get_if_copy_paste() const { return (this->is_copy_paste()) ? reinterpret_cast<const std::string*>(this->m_variant) : nullptr; }

  // constructs a Key from the contained Key if it is one and otherwise returns a default one
  inline operator Term::Key() const { return (this->is_key()) ? *reinterpret_cast<const Key*>(this->m_variant) : Key(); }

  // constructs a Screen from the contained Screen if it is one and otherwise returns a default one
  inline operator Term::Screen() const { return (this->is_screen()) ? *reinterpret_cast<const Screen*>(this->m_variant) : Screen(); }

  // constructs a Cursor from the contained Cursor if it is one and otherwise returns a default one
  inline operator Term::Cursor() const { return (this->is_cursor()) ? *reinterpret_cast<const Cursor*>(this->m_variant) : Cursor(); }

  // constructs a std::string from the contained Copy Paste Event if it is one and otherwise returns a default one
  inline operator std::string() const { return (this->is_copy_paste()) ? *reinterpret_cast<const std::string*>(this->m_variant) : std::string(); }

private:
#define temp_max_2(a, b)       (((a) > (b)) ? (a) : (b))
#define temp_max_4(a, b, c, d) temp_max_2(temp_max_2(a, b), temp_max_2(c, d))

#define MAX_ALIGN temp_max_4(alignof(std::string), alignof(Key), alignof(Cursor), alignof(Term::Screen))
#define MAX_SIZE  temp_max_4(sizeof(std::string), sizeof(Key), sizeof(Cursor), sizeof(Term::Screen))

  alignas(MAX_ALIGN) uint8_t m_variant[MAX_SIZE];

#undef temp_max_2
#undef temp_max_4
#undef MAX_ALIGN
#undef MAX_SIZE

  Type m_Type{Type::Empty};
};

}  // namespace Term
