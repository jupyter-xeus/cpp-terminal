#include "cpp-terminal/event.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

// copy constructs an event
Term::Event::Event(const Term::Event& event) : m_Type(event.m_Type)
{
  switch(event.m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: new(reinterpret_cast<void*>(this->m_variant)) Term::Key(*reinterpret_cast<const Key*>(event.m_variant)); break;
    case Term::Event::Type::Screen: new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(*reinterpret_cast<const Screen*>(event.m_variant)); break;
    case Term::Event::Type::Cursor: new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(*reinterpret_cast<const Cursor*>(event.m_variant)); break;
    case Term::Event::Type::CopyPaste: new(reinterpret_cast<void*>(this->m_variant)) std::string(*reinterpret_cast<const std::string*>(event.m_variant)); break;
  }
}

// move constructs an event, which makes the other event an empty event
Term::Event::Event(Term::Event&& event) : m_Type(event.m_Type)
{
  switch(event.m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: new(reinterpret_cast<void*>(this->m_variant)) Term::Key(std::move(*reinterpret_cast<Key*>(event.m_variant))); break;
    case Term::Event::Type::Screen: new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(std::move(*reinterpret_cast<Screen*>(event.m_variant))); break;
    case Term::Event::Type::Cursor: new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(std::move(*reinterpret_cast<Cursor*>(event.m_variant))); break;
    case Term::Event::Type::CopyPaste: new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(*reinterpret_cast<std::string*>(event.m_variant))); break;
  }
  event.m_Type = Term::Event::Type::Empty;
}

// construcst a key event
Term::Event::Event(const Term::Key& key) : m_Type(Term::Event::Type::Key) { new(reinterpret_cast<void*>(this->m_variant)) Term::Key(key); }

// constructs a screen event
Term::Event::Event(const Term::Screen& screen) : m_Type(Term::Event::Type::Screen) { new(reinterpret_cast<void*>(this->m_variant)) Term::Screen(screen); }

// constructs a cursor event
Term::Event::Event(const Term::Cursor& cursor) : m_Type(Term::Event::Type::Cursor) { new(reinterpret_cast<void*>(this->m_variant)) Term::Cursor(cursor); }

// constructs a copy paste event by copying the string
Term::Event::Event(const std::string& str) : m_Type(Term::Event::Type::CopyPaste) { new(reinterpret_cast<void*>(this->m_variant)) std::string(str); }

// constructs a copy paste event by moveing the string
Term::Event::Event(std::string&& str) : m_Type(Term::Event::Type::CopyPaste) { new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(str)); }

// safely deconstrucst an event by calling the destructor of the stored type
Term::Event::~Event() { this->clear(); }

// assigning an event will call the destructor if the types differ and assign directly if the types are equal
Term::Event& Term::Event::operator=(const Term::Event& event) { return this->assign(event); }

// assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
Term::Event& Term::Event::operator=(Term::Event&& event) { return this->assign(std::move(event)); }

// assigning an event will call the destructor if the types differ and assign directly if the types are equal
Term::Event& Term::Event::assign(const Term::Event& event)
{
  switch(event.m_Type)
  {
    case Term::Event::Type::Empty: this->clear(); break;
    case Term::Event::Type::Key: this->assign(*reinterpret_cast<const Term::Key*>(event.m_variant)); break;
    case Term::Event::Type::Screen: this->assign(*reinterpret_cast<const Term::Screen*>(event.m_variant)); break;
    case Term::Event::Type::Cursor: this->assign(*reinterpret_cast<const Term::Cursor*>(event.m_variant)); break;
    case Term::Event::Type::CopyPaste: this->assign(*reinterpret_cast<const std::string*>(event.m_variant)); break;
  }
  return *this;
}

// assigning an event will call the destructor if the types differ and assign directly if the types are equal. make the other event an empty event
Term::Event& Term::Event::assign(Term::Event&& event)
{
  switch(event.m_Type)
  {
    case Term::Event::Type::Empty: this->clear(); break;
    case Term::Event::Type::Key: this->assign(std::move(*reinterpret_cast<Term::Key*>(event.m_variant))); break;
    case Term::Event::Type::Screen: this->assign(std::move(*reinterpret_cast<Term::Screen*>(event.m_variant))); break;
    case Term::Event::Type::Cursor: this->assign(std::move(*reinterpret_cast<Term::Cursor*>(event.m_variant))); break;
    case Term::Event::Type::CopyPaste: this->assign(std::move(*reinterpret_cast<std::string*>(event.m_variant))); break;
  }
  event.m_Type = Term::Event::Type::Empty;
  return *this;
}

// copy assigns a key event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
Term::Event& Term::Event::assign(const Term::Key& key)
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: *reinterpret_cast<Term::Key*>(this->m_variant) = key; return *this;
    case Term::Event::Type::Screen: reinterpret_cast<Term::Screen*>(this->m_variant)->~Screen(); break;
    case Term::Event::Type::Cursor: reinterpret_cast<Term::Cursor*>(this->m_variant)->~Cursor(); break;
    case Term::Event::Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
  }
  new(reinterpret_cast<void*>(this->m_variant)) Key(key);
  this->m_Type = Term::Event::Type::Key;
  return *this;
}

// copy assigns a screen event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
Term::Event& Term::Event::assign(const Term::Screen& screen)
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: reinterpret_cast<Term::Key*>(this->m_variant)->~Key(); break;
    case Term::Event::Type::Screen:
      *reinterpret_cast<Screen*>(this->m_variant) = screen;
      return *this;
      break;
    case Term::Event::Type::Cursor: reinterpret_cast<Term::Cursor*>(this->m_variant)->~Cursor(); break;
    case Term::Event::Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
  }
  new(reinterpret_cast<void*>(this->m_variant)) Screen(screen);
  this->m_Type = Term::Event::Type::Screen;
  return *this;
}

// copy assigns a cursor event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
Term::Event& Term::Event::assign(const Term::Cursor& cursor)
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: reinterpret_cast<Term::Key*>(this->m_variant)->~Key(); break;
    case Term::Event::Type::Screen: reinterpret_cast<Term::Screen*>(this->m_variant)->~Screen(); break;
    case Term::Event::Type::Cursor:
      *reinterpret_cast<Term::Cursor*>(this->m_variant) = cursor;
      return *this;
      break;
    case Term::Event::Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
  }
  new(reinterpret_cast<void*>(this->m_variant)) Cursor(cursor);
  this->m_Type = Term::Event::Type::Cursor;
  return *this;
}

// copy assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
Term::Event& Term::Event::assign(const std::string& str)
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: reinterpret_cast<Term::Key*>(this->m_variant)->~Key(); break;
    case Term::Event::Type::Screen: reinterpret_cast<Term::Screen*>(this->m_variant)->~Screen(); break;
    case Term::Event::Type::Cursor: reinterpret_cast<Term::Cursor*>(this->m_variant)->~Cursor(); break;
    case Term::Event::Type::CopyPaste:
      reinterpret_cast<std::string*>(this->m_variant)->assign(str);
      return *this;
      break;
  }
  new(reinterpret_cast<void*>(this->m_variant)) std::string(str);
  this->m_Type = Term::Event::Type::CopyPaste;
  return *this;
}

// move assigns a copy paste event. call the destructor and copy constructs a new key if this is not a the same type, otherwise assigns directly
Term::Event& Term::Event::assign(std::string&& str)
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: reinterpret_cast<Term::Key*>(this->m_variant)->~Key(); break;
    case Term::Event::Type::Screen: reinterpret_cast<Term::Screen*>(this->m_variant)->~Screen(); break;
    case Term::Event::Type::Cursor: reinterpret_cast<Term::Cursor*>(this->m_variant)->~Cursor(); break;
    case Term::Event::Type::CopyPaste:
      reinterpret_cast<std::string*>(this->m_variant)->assign(std::move(str));
      return *this;
      break;
  }
  new(reinterpret_cast<void*>(this->m_variant)) std::string(std::move(str));
  this->m_Type = Term::Event::Type::CopyPaste;
  return *this;
}

// destructs the containing type and makes this an empty type
Term::Event& Term::Event::clear()
{
  switch(this->m_Type)
  {
    case Term::Event::Type::Empty: break;
    case Term::Event::Type::Key: reinterpret_cast<Term::Key*>(this->m_variant)->~Key(); break;
    case Term::Event::Type::Screen: reinterpret_cast<Term::Screen*>(this->m_variant)->~Screen(); break;
    case Term::Event::Type::Cursor: reinterpret_cast<Term::Cursor*>(this->m_variant)->~Cursor(); break;
    case Term::Event::Type::CopyPaste: reinterpret_cast<std::string*>(this->m_variant)->~basic_string(); break;
  }
  this->m_Type = Term::Event::Type::Empty;
  return *this;
}

// returns true if this is an empty type
bool Term::Event::empty() const { return this->m_Type == Term::Event::Type::Empty; }

// returns the contained type enum
Term::Event::Type Term::Event::type() const { return this->m_Type; };

// returns true if the contained type is an empty type
bool Term::Event::is_empty() const { return this->m_Type == Term::Event::Type::Empty; }

// returns true if the contained type is a key type
bool Term::Event::is_key() const { return this->m_Type == Term::Event::Type::Key; }

// returns true if the contained type is a screen type
bool Term::Event::is_screen() const { return this->m_Type == Term::Event::Type::Screen; }

// returns true if the contained type is a cursor type
bool Term::Event::is_cursor() const { return this->m_Type == Term::Event::Type::Cursor; }

// returns true if the contained type is a copy paset type
bool Term::Event::is_copy_paste() const { return this->m_Type == Term::Event::Type::CopyPaste; }

// returns a pointer to the contained key type if it is one and otherwise a nullptr
Term::Key* Term::Event::get_if_key() { return (this->is_key()) ? reinterpret_cast<Term::Key*>(this->m_variant) : nullptr; }

// returns a const pointer to the contained key type if it is one and otherwise a nullptr
const Term::Key* Term::Event::get_if_key() const { return (this->is_key()) ? reinterpret_cast<const Term::Key*>(this->m_variant) : nullptr; }

// returns a pointer to the contained screen type if it is one and otherwise a nullptr
Term::Screen* Term::Event::get_if_screen() { return (this->is_screen()) ? reinterpret_cast<Term::Screen*>(this->m_variant) : nullptr; }

// returns a const pointer to the contained screen type if it is one and otherwise a nullptr
const Term::Screen* Term::Event::get_if_screen() const { return (this->is_screen()) ? reinterpret_cast<const Term::Screen*>(this->m_variant) : nullptr; }

// returns a pointer to the contained cursor type if it is one and otherwise a nullptr
Term::Cursor* Term::Event::get_if_cursor() { return (this->is_cursor()) ? reinterpret_cast<Term::Cursor*>(this->m_variant) : nullptr; }

// returns a const pointer to the contained cursor type if it is one and otherwise a nullptr
const Term::Cursor* Term::Event::get_if_cursor() const { return (this->is_cursor()) ? reinterpret_cast<const Term::Cursor*>(this->m_variant) : nullptr; }

// returns a pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
std::string* Term::Event::get_if_copy_paste() { return (this->is_copy_paste()) ? reinterpret_cast<std::string*>(this->m_variant) : nullptr; }

// returns a const pointer to the contained copy paste type (aka string) if it is one and otherwise a nullptr
const std::string* Term::Event::get_if_copy_paste() const { return (this->is_copy_paste()) ? reinterpret_cast<const std::string*>(this->m_variant) : nullptr; }

// constructs a Key from the contained Key if it is one and otherwise returns a default one
Term::Event::operator Term::Key() const { return (this->is_key()) ? *reinterpret_cast<const Term::Key*>(this->m_variant) : Key(); }

// constructs a Screen from the contained Screen if it is one and otherwise returns a default one
Term::Event::operator Term::Screen() const { return (this->is_screen()) ? *reinterpret_cast<const Term::Screen*>(this->m_variant) : Screen(); }

// constructs a Cursor from the contained Cursor if it is one and otherwise returns a default one
Term::Event::operator Term::Cursor() const { return (this->is_cursor()) ? *reinterpret_cast<const Term::Cursor*>(this->m_variant) : Cursor(); }

// constructs a std::string from the contained Copy Paste Event if it is one and otherwise returns a default one
Term::Event::operator std::string() const { return (this->is_copy_paste()) ? *reinterpret_cast<const std::string*>(this->m_variant) : std::string(); }

Term::Event Term::parse_event(std::string&& str)
{
  if (str.size() == 0) {
    return Term::Event();
  }
  else if(str.size() == 1)
  {
    Term::Key key = Key(static_cast<Term::Key::Value>(str[0]));
    /* Backspace return 127 CTRL+backspace return 8 */
    if(key == Term::Key::Value::DEL) return Term::Event(Key(Term::Key::Value::BACKSPACE));
    else
      return Term::Event(key);
  }
  else if(str.size() == 2 && str[0] == '\033') { return Term::Event(Key(static_cast<Term::Key::Value>(Term::Key::Value::ALT + static_cast<Term::Key::Value>(str[1])))); }
  else if(str[0] == '\033' && str[1] == '[' && str[str.size() - 1] == 'R')
  {
    std::size_t found = str.find(';', 2);
    if(found != std::string::npos)
    {
      const auto substr1 = str.substr(std::size_t(2), found - std::size_t(2));
      const auto num1    = std::stoul(substr1);
      const auto substr2 = str.substr(found + std::size_t(1), str.size() - (found + std::size_t(2)));
      const auto num2    = std::stoul(substr2);
      return Event(Cursor(num1, num2));
    }
    else { return Event(); }
  }
  else if(str.size() <= 10)
  {
    //https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
    // CSI = ESC[ SS3 = ESCO
    /*
     * Key            Normal     Application
     * -------------+----------+-------------
     * Cursor Up    | CSI A    | SS3 A
     * Cursor Down  | CSI B    | SS3 B
     * Cursor Right | CSI C    | SS3 C
     * Cursor Left  | CSI D    | SS3 D
     * -------------+----------+-------------
     * Key            Normal/Application
     * -------------+--------------------
     * Cursor Up    | ESC A
     * Cursor Down  | ESC B
     * Cursor Right | ESC C
     * Cursor Left  | ESC D
     * -------------+--------------------
    */
    if(str == "\033OA" || str == "\033[A" || str == "\033A") return Term::Event(Term::Key(Term::Key::Value::ARROW_UP));
    else if(str == "\033OB" || str == "\033[B" || str == "\033B")
      return Term::Event(Key(Term::Key::Value::ARROW_DOWN));
    else if(str == "\033OC" || str == "\033[C" || str == "\033C")
      return Term::Event(Key(Term::Key::Value::ARROW_RIGHT));
    else if(str == "\033OD" || str == "\033[D" || str == "\033D")
      return Term::Event(Key(Term::Key::Value::ARROW_LEFT));
    /*
     * Key        Normal     Application
     * ---------+----------+-------------
     * Home     | CSI H    | SS3 H
     * End      | CSI F    | SS3 F
     * ---------+----------+-------------
    */
    else if(str == "\033OH" || str == "\033[H")
      return Term::Event(Key(Term::Key::Value::HOME));
    else if(str == "\033OF" || str == "\033[F")
      return Term::Event(Key(Term::Key::Value::END));
    /*
     * Key        Escape Sequence
     * ---------+-----------------
     * F1       | SS3 P
     * F2       | SS3 Q
     * F3       | SS3 R
     * F4       | SS3 S
     * F1       | CSI 1 1 ~
     * F2       | CSI 1 2 ~
     * F3       | CSI 1 3 ~
     * F4       | CSI 1 4 ~
     * F5       | CSI 1 5 ~
     * F6       | CSI 1 7 ~
     * F7       | CSI 1 8 ~
     * F8       | CSI 1 9 ~
     * F9       | CSI 2 0 ~
     * F10      | CSI 2 1 ~
     * F11      | CSI 2 3 ~
     * F12      | CSI 2 4 ~
     * ---------+-----------------
    */
    else if(str == "\033OP" || str == "\033[11~")
      return Term::Event(Key(Term::Key::Value::F1));
    else if(str == "\033OQ" || str == "\033[12~")
      return Term::Event(Key(Term::Key::Value::F2));
    else if(str == "\033OR" || str == "\033[13~")
      return Term::Event(Key(Term::Key::Value::F3));
    else if(str == "\033OS" || str == "\033[14~")
      return Term::Event(Key(Term::Key::Value::F4));
    else if(str == "\033[15~")
      return Term::Event(Key(Term::Key::Value::F5));
    else if(str == "\033[17~")
      return Term::Event(Key(Term::Key::Value::F6));
    else if(str == "\033[18~")
      return Term::Event(Key(Term::Key::Value::F7));
    else if(str == "\033[19~")
      return Term::Event(Key(Term::Key::Value::F8));
    else if(str == "\033[20~")
      return Term::Event(Key(Term::Key::Value::F9));
    else if(str == "\033[21~")
      return Term::Event(Key(Term::Key::Value::F10));
    else if(str == "\033[23~")
      return Term::Event(Key(Term::Key::Value::F11));
    else if(str == "\033[24~")
      return Term::Event(Key(Term::Key::Value::F12));
    /*
     * Key        Normal     Application
     * ---------+----------+-------------
     * Insert   | CSI 2 ~  | CSI 2 ~
     * Delete   | CSI 3 ~  | CSI 3 ~
     * Home     | CSI 1 ~  | CSI 1 ~
     * End      | CSI 4 ~  | CSI 4 ~
     * PageUp   | CSI 5 ~  | CSI 5 ~
     * PageDown | CSI 6 ~  | CSI 6 ~
     * ---------+----------+-------------
    */
    else if(str == "\033[2~")
      return Term::Event(Key(Term::Key::Value::INSERT));
    else if(str == "\033[3~")
      return Term::Event(Key(Term::Key::Value::DEL));
    else if(str == "\033[1~")
      return Term::Event(Key(Term::Key::Value::HOME));
    else if(str == "\033[4~")
      return Term::Event(Key(Term::Key::Value::END));
    else if(str == "\033[5~")
      return Term::Event(Key(Term::Key::Value::PAGE_UP));
    else if(str == "\033[6~")
      return Term::Event(Key(Term::Key::Value::PAGE_DOWN));
    /*
     * Key        Escape Sequence
     * ---------+-----------------
     * F13      | CSI 2 5 ~
     * F14      | CSI 2 6 ~
     * F15      | CSI 2 8 ~
     * F16      | CSI 2 9 ~
     * F17      | CSI 3 1 ~
     * F18      | CSI 3 2 ~
     * F19      | CSI 3 3 ~
     * F20      | CSI 3 4 ~
     * ---------+-----------------
    */
    else if(str == "\033[25~")
      return Term::Event(Key(Term::Key::Value::F13));
    else if(str == "\033[26~")
      return Term::Event(Key(Term::Key::Value::F14));
    else if(str == "\033[28~")
      return Term::Event(Key(Term::Key::Value::F15));
    else if(str == "\033[29~")
      return Term::Event(Key(Term::Key::Value::F16));
    else if(str == "\033[31~")
      return Term::Event(Key(Term::Key::Value::F17));
    else if(str == "\033[32~")
      return Term::Event(Key(Term::Key::Value::F18));
    else if(str == "\033[33~")
      return Term::Event(Key(Term::Key::Value::F19));
    else if(str == "\033[34~")
      return Term::Event(Key(Term::Key::Value::F20));
    else if(str == "\033[G")
      return Term::Event(Key(Term::Key::Value::NUMERIC_5));
    else if(str.size() == 2 && ((str[0] & 0xE0) == 0xC0) && ((str[1] & 0xC0) == 0x80))
      return Term::Event(Key(static_cast<Term::Key::Value>(Term::Private::utf8_to_utf32(str)[0])));
    else if(str.size() == 3 && ((str[0] & 0xF0) == 0xE0) && ((str[1] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80))
      return Term::Event(Key(static_cast<Term::Key::Value>(Term::Private::utf8_to_utf32(str)[0])));
    else if(str.size() == 4 && ((str[0] & 0xF8) == 0xF0) && ((str[1] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80))
      return Term::Event(Key(static_cast<Term::Key::Value>(Term::Private::utf8_to_utf32(str)[0])));
  }
  return Term::Event(std::move(str));
}
