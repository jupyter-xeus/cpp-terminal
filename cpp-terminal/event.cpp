#include "cpp-terminal/event.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

#include <cstring>

Term::Event::container::container()
{
  m_Key    = {};
  m_Cursor = {};
  m_Screen = {};
  m_string = {nullptr};
}

Term::Event::container::~container() {}

Term::Key* Term::Event::get_if_key()
{
  if(m_Type == Type::Key) return &m_container.m_Key;
  else
    return nullptr;
}

const Term::Key* Term::Event::get_if_key() const
{
  if(m_Type == Type::Key) return &m_container.m_Key;
  else
    return nullptr;
}

Term::Screen* Term::Event::get_if_screen()
{
  if(m_Type == Type::Screen) return &m_container.m_Screen;
  else
    return nullptr;
}

const Term::Screen* Term::Event::get_if_screen() const
{
  if(m_Type == Type::Screen) return &m_container.m_Screen;
  else
    return nullptr;
}

Term::Cursor* Term::Event::get_if_cursor()
{
  if(m_Type == Type::Cursor) return &m_container.m_Cursor;
  else
    return nullptr;
}

const Term::Cursor* Term::Event::get_if_cursor() const
{
  if(m_Type == Type::Cursor) return &m_container.m_Cursor;
  else
    return nullptr;
}

std::string Term::Event::get_if_copy_paste()
{
  if(m_Type == Type::CopyPaste) return std::string(m_container.m_string.get());
  else
    return nullptr;
}

const std::string Term::Event::get_if_copy_paste() const
{
  if(m_Type == Type::CopyPaste) return std::string(m_container.m_string.get());
  else
    return nullptr;
}

Term::Event& Term::Event::operator=(const Term::Event& event)
{
  m_Type = event.m_Type;
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: m_container.m_Key = Term::Key(event.m_container.m_Key); break;
    case Type::CopyPaste:
    {
      m_container.m_string.reset(new char[std::strlen(event.m_container.m_string.get())]);
#if defined(_WIN32)
      strcpy_s(m_container.m_string.get(), std::strlen(m_container.m_string.get()), event.m_container.m_string.get());
#else
      strcpy(m_container.m_string.get(), event.m_container.m_string.get());
#endif
      break;
    }
    case Type::Cursor: m_container.m_Cursor = Term::Cursor(event.m_container.m_Cursor); break;
    case Type::Screen: m_container.m_Screen = Term::Screen(event.m_container.m_Screen); break;
  }
  return *this;
}

Term::Event::Event(const Term::Event& event)
{
  m_Type = event.m_Type;
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: m_container.m_Key = Term::Key(event.m_container.m_Key); break;
    case Type::CopyPaste:
    {
      m_container.m_string.reset(new char[std::strlen(event.m_container.m_string.get())]);
#if defined(_WIN32)
      strcpy_s(m_container.m_string.get(), std::strlen(m_container.m_string.get()), event.m_container.m_string.get());
#else
      strcpy(m_container.m_string.get(), event.m_container.m_string.get());
#endif
      break;
    }
    case Type::Cursor: m_container.m_Cursor = Term::Cursor(event.m_container.m_Cursor); break;
    case Type::Screen: m_container.m_Screen = Term::Screen(event.m_container.m_Screen); break;
  }
}

Term::Event::~Event() {}

Term::Event::Event() : m_Type(Type::Empty) {}

Term::Event::Event(Term::Event&& event) noexcept
{
  m_Type = std::move(event.m_Type);
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: std::swap(m_container.m_Key, event.m_container.m_Key); break;
    case Type::CopyPaste:
    {
      std::swap(m_container.m_string, event.m_container.m_string);
      break;
    }
    case Type::Cursor: std::swap(m_container.m_Cursor, event.m_container.m_Cursor); break;
    case Type::Screen: std::swap(m_container.m_Screen, event.m_container.m_Screen); break;
  }
}

Term::Event& Term::Event::operator=(Term::Event&& event) noexcept
{
  m_Type = std::move(event.m_Type);
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: std::swap(m_container.m_Key, event.m_container.m_Key); break;
    case Type::CopyPaste: std::swap(m_container.m_string, event.m_container.m_string); break;
    case Type::Cursor: std::swap(m_container.m_Cursor, event.m_container.m_Cursor); break;
    case Type::Screen: std::swap(m_container.m_Screen, event.m_container.m_Screen); break;
  }
  return *this;
}

bool Term::Event::empty() const
{
  if(m_Type == Type::Empty) return true;
  else
    return false;
}

Term::Event::operator std::string() const
{
  if(m_Type == Type::CopyPaste) return m_container.m_string.get();
  else
    return std::string();
}

Term::Event::operator Term::Screen() const
{
  if(m_Type == Type::Screen) return m_container.m_Screen;
  else
    return Term::Screen();
}

Term::Event::Event(const Term::Screen& screen) : m_Type(Type::Screen) { m_container.m_Screen = screen; }

Term::Event::Event(const Term::Key& key) : m_Type(Type::Key) { m_container.m_Key = key; }

Term::Event::Type Term::Event::type() const { return m_Type; }

Term::Event::Event(const std::string& str) : m_Type(Type::CopyPaste) { parse(str); }

void Term::Event::parse(const std::string& str)
{
  if(str.empty()) { m_Type = Type::Empty; }
  else if(str.size() == 1)
  {
    m_Type            = Type::Key;
    m_container.m_Key = Key(static_cast<Term::Key::Value>(str[0]));
    /* Backspace return 127 CTRL+backspace return 8 */
    if(m_container.m_Key == Term::Key::Value::Del) m_container.m_Key = Key(Term::Key::Value::Backspace);
  }
  else if(str.size() == 2 && str[0] == '\033')
  {
    m_container.m_Key = Key(static_cast<Term::Key::Value>(Term::MetaKey::Value::Alt + static_cast<Term::Key::Value>(str[1])));
    m_Type            = Type::Key;
  }
  else if(str[0] == '\033' && str[1] == '[' && str[str.size() - 1] == 'R')
  {
    std::size_t found = str.find(';', 2);
    if(found != std::string::npos)
    {
      m_Type               = Type::Cursor;
      m_container.m_Cursor = Cursor(static_cast<std::uint16_t>(std::stoi(str.substr(2, found - 2))), static_cast<std::uint16_t>(std::stoi(str.substr(found + 1, str.size() - (found + 2)))));
    }
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
    if(str == "\033OA" || str == "\033[A" || str == "\033A") m_container.m_Key = Key(Term::Key::Value::ArrowUp);
    else if(str == "\033OB" || str == "\033[B" || str == "\033B")
      m_container.m_Key = Key(Term::Key::Value::ArrowDown);
    else if(str == "\033OC" || str == "\033[C" || str == "\033C")
      m_container.m_Key = Key(Term::Key::Value::ArrowRight);
    else if(str == "\033OD" || str == "\033[D" || str == "\033D")
      m_container.m_Key = Key(Term::Key::Value::ArrowLeft);
    /*
     * Key        Normal     Application
     * ---------+----------+-------------
     * Home     | CSI H    | SS3 H
     * End      | CSI F    | SS3 F
     * ---------+----------+-------------
    */
    else if(str == "\033OH" || str == "\033[H")
      m_container.m_Key = Key(Term::Key::Value::Home);
    else if(str == "\033OF" || str == "\033[F")
      m_container.m_Key = Key(Term::Key::Value::End);
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
      m_container.m_Key = Key(Term::Key::Value::F1);
    else if(str == "\033OQ" || str == "\033[12~")
      m_container.m_Key = Key(Term::Key::Value::F2);
    else if(str == "\033OR" || str == "\033[13~")
      m_container.m_Key = Key(Term::Key::Value::F3);
    else if(str == "\033OS" || str == "\033[14~")
      m_container.m_Key = Key(Term::Key::Value::F4);
    else if(str == "\033[15~")
      m_container.m_Key = Key(Term::Key::Value::F5);
    else if(str == "\033[17~")
      m_container.m_Key = Key(Term::Key::Value::F6);
    else if(str == "\033[18~")
      m_container.m_Key = Key(Term::Key::Value::F7);
    else if(str == "\033[19~")
      m_container.m_Key = Key(Term::Key::Value::F8);
    else if(str == "\033[20~")
      m_container.m_Key = Key(Term::Key::Value::F9);
    else if(str == "\033[21~")
      m_container.m_Key = Key(Term::Key::Value::F10);
    else if(str == "\033[23~")
      m_container.m_Key = Key(Term::Key::Value::F11);
    else if(str == "\033[24~")
      m_container.m_Key = Key(Term::Key::Value::F12);
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
      m_container.m_Key = Key(Term::Key::Value::Insert);
    else if(str == "\033[3~")
      m_container.m_Key = Key(Term::Key::Value::Del);
    else if(str == "\033[1~")
      m_container.m_Key = Key(Term::Key::Value::Home);
    else if(str == "\033[4~")
      m_container.m_Key = Key(Term::Key::Value::End);
    else if(str == "\033[5~")
      m_container.m_Key = Key(Term::Key::Value::PageUp);
    else if(str == "\033[6~")
      m_container.m_Key = Key(Term::Key::Value::PageDown);
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
      m_container.m_Key = Key(Term::Key::Value::F13);
    else if(str == "\033[26~")
      m_container.m_Key = Key(Term::Key::Value::F14);
    else if(str == "\033[28~")
      m_container.m_Key = Key(Term::Key::Value::F15);
    else if(str == "\033[29~")
      m_container.m_Key = Key(Term::Key::Value::F16);
    else if(str == "\033[31~")
      m_container.m_Key = Key(Term::Key::Value::F17);
    else if(str == "\033[32~")
      m_container.m_Key = Key(Term::Key::Value::F18);
    else if(str == "\033[33~")
      m_container.m_Key = Key(Term::Key::Value::F19);
    else if(str == "\033[34~")
      m_container.m_Key = Key(Term::Key::Value::F20);
    else if(str == "\033[G")
      m_container.m_Key = Key(Term::Key::Value::Numeric5);
    else if(Term::Private::is_valid_utf8_code_unit(str))
      m_container.m_Key = Key(static_cast<Term::Key::Value>(Term::Private::utf8_to_utf32(str)[0]));
    else
    {
      m_Type = Type::CopyPaste;
      m_container.m_string.reset(new char[str.size() + 1]);
      std::copy(str.begin(), str.end(), m_container.m_string.get());
      m_container.m_string.get()[str.size()] = '\0';
      return;
    }
    m_Type = Type::Key;
  }
  else
  {
    m_Type = Type::CopyPaste;
    m_container.m_string.reset(new char[str.size() + 1]);
    std::copy(str.begin(), str.end(), m_container.m_string.get());
    m_container.m_string.get()[str.size()] = '\0';
  }
}

Term::Event::operator Term::Key() const
{
  if(m_Type == Type::Key) return m_container.m_Key;
  else
    return Key();
}

Term::Event::operator Term::Cursor() const
{
  if(m_Type == Type::Cursor) return m_container.m_Cursor;
  else
    return Cursor();
}
