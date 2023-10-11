/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/event.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

#if defined(_MSC_VER)
  // Disable stupid warnings on Windows
  #pragma warning(push)
  #pragma warning(disable : 4582)
  #pragma warning(disable : 4583)
#endif
Term::Event::container::container() {}

Term::Event::container::~container() {}
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

#include <chrono>

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

Term::Mouse* Term::Event::get_if_mouse()
{
  if(m_Type == Type::Mouse) return &m_container.m_Mouse;
  else
    return nullptr;
}

const Term::Mouse* Term::Event::get_if_mouse() const
{
  if(m_Type == Type::Mouse) return &m_container.m_Mouse;
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

std::string* Term::Event::get_if_copy_paste()
{
  if(m_Type == Type::CopyPaste) return &m_container.m_string;
  return nullptr;
}

const std::string* Term::Event::get_if_copy_paste() const
{
  if(m_Type == Type::CopyPaste) return &m_container.m_string;
  return nullptr;
}

Term::Event::Event(const Term::Cursor& cursor) : m_Type(Type::Cursor) { m_container.m_Cursor = cursor; }

Term::Focus* Term::Event::get_if_focus()
{
  if(m_Type == Type::Focus) return &m_container.m_Focus;
  else
    return nullptr;
}

const Term::Focus* Term::Event::get_if_focus() const
{
  if(m_Type == Type::Focus) return &m_container.m_Focus;
  else
    return nullptr;
}

Term::Event& Term::Event::operator=(const Term::Event& event)
{
  m_Type = event.m_Type;
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: m_container.m_Key = event.m_container.m_Key; break;
    case Type::CopyPaste: new(&this->m_container.m_string) std::string(event.m_container.m_string); break;
    case Type::Cursor: m_container.m_Cursor = event.m_container.m_Cursor; break;
    case Type::Screen: m_container.m_Screen = event.m_container.m_Screen; break;
    case Type::Focus: m_container.m_Focus = event.m_container.m_Focus; break;
    case Type::Mouse: m_container.m_Mouse = event.m_container.m_Mouse; break;
  }
  return *this;
}

Term::Event::Event(const Term::Focus& focus) : m_Type(Type::Focus) { m_container.m_Focus = focus; }

Term::Event::Event(const Term::Event& event)
{
  m_Type = event.m_Type;
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: m_container.m_Key = event.m_container.m_Key; break;
    case Type::CopyPaste: new(&this->m_container.m_string) std::string(event.m_container.m_string); break;
    case Type::Cursor: m_container.m_Cursor = event.m_container.m_Cursor; break;
    case Type::Screen: m_container.m_Screen = event.m_container.m_Screen; break;
    case Type::Focus: m_container.m_Focus = event.m_container.m_Focus; break;
    case Type::Mouse: m_container.m_Mouse = event.m_container.m_Mouse; break;
  }
}

Term::Event::~Event()
{
  using std::string;
  if(m_Type == Type::CopyPaste) m_container.m_string.~string();
}

Term::Event::Event() : m_Type(Type::Empty) {}

Term::Event::Event(Term::Event&& event) noexcept
{
  m_Type = std::move(event.m_Type);
  switch(m_Type)
  {
    case Type::Empty: break;
    case Type::Key: std::swap(m_container.m_Key, event.m_container.m_Key); break;
    case Type::CopyPaste: std::swap(m_container.m_string, event.m_container.m_string); break;
    case Type::Cursor: std::swap(m_container.m_Cursor, event.m_container.m_Cursor); break;
    case Type::Screen: std::swap(m_container.m_Screen, event.m_container.m_Screen); break;
    case Type::Focus: std::swap(m_container.m_Focus, event.m_container.m_Focus); break;
    case Type::Mouse: std::swap(m_container.m_Mouse, event.m_container.m_Mouse); break;
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
    case Type::Focus: std::swap(m_container.m_Focus, event.m_container.m_Focus); break;
    case Type::Mouse: std::swap(m_container.m_Mouse, event.m_container.m_Mouse); break;
  }
  return *this;
}

Term::Event::Event(const Term::Mouse& mouse) : m_Type(Type::Mouse) { m_container.m_Mouse = mouse; }

bool Term::Event::empty() const
{
  if(m_Type == Type::Empty) return true;
  else
    return false;
}

Term::Event::operator Term::Mouse() const
{
  if(m_Type == Type::Mouse) return m_container.m_Mouse;
  else
    return Mouse();
}

Term::Event::operator std::string() const
{
  if(m_Type == Type::CopyPaste) return m_container.m_string;
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

Term::Event::Event(const std::string& str) : m_Type(Type::Empty) { parse(str); }

void Term::Event::parse(const std::string& str)
{
  if(str.empty()) m_Type = Type::Empty;
  else if(str.size() == 1)
  {
    m_Type            = Type::Key;
    m_container.m_Key = Key(static_cast<Term::Key>(str[0]));
    /* Backspace return 127 CTRL+backspace return 8 */
    if(m_container.m_Key == Term::Key::Del) m_container.m_Key = Key(Term::Key::Backspace);
  }
  else if(str == "\033[I")
  {
    m_Type              = Type::Focus;
    m_container.m_Focus = Term::Focus(Term::Focus::Type::In);
  }
  else if(str == "\033[O")
  {
    m_Type              = Type::Focus;
    m_container.m_Focus = Term::Focus(Term::Focus::Type::Out);
  }
  else if(str.size() == 2 && str[0] == '\033')
  {
    m_container.m_Key = Key(static_cast<Term::Key>(Term::MetaKey::Value::Alt + static_cast<Term::Key>(str[1])));
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
  else if(str[0] == '\033' && str[1] == '[' && str[2] == '<')
  {
    static std::chrono::time_point<std::chrono::system_clock> old;
    bool                                                      not_too_long{false};
    if(std::chrono::system_clock::now() - old <= std::chrono::milliseconds{120}) not_too_long = true;
    m_Type = Type::Mouse;
    std::size_t              pos{3};
    std::size_t              pos2{3};
    std::vector<std::size_t> values;
    while((pos = str.find(';', pos)) != std::string::npos)
    {
      values.push_back(std::stoull(str.substr(pos2, pos - pos2)));
      pos++;
      pos2 = pos;
    }
    values.push_back(std::stoull(str.substr(pos2, str.size() - pos2 - 1)));
    static Term::Mouse   first;
    static Term::Mouse   second;
    Term::Button::Action action;
    if(str[str.size() - 1] == 'm') action = Term::Button::Action::Released;
    else
      action = Term::Button::Action::Pressed;
    Term::Button::Type type;
    switch(values[0])
    {
      case 0:
      {
        type = Term::Button::Type::Right;
        break;
      }
      case 1:
      {
        type = Term::Button::Type::Wheel;
        break;
      }
      case 2:
      {
        type = Term::Button::Type::Left;
        break;
      }
      case 35:
      {
        type   = Term::Button::Type::None;
        action = Term::Button::Action::None;
        break;
      }
      case 64:
      {
        type   = Term::Button::Type::Wheel;
        action = Term::Button::Action::RolledUp;
        break;
      }
      case 65:
      {
        type   = Term::Button::Type::Wheel;
        action = Term::Button::Action::RolledDown;
        break;
      }
      default: break;
    }
    if(not_too_long && first.row() == second.row() && second.row() == values[1] && first.column() == second.column() && second.column() == values[2] && first.getButton().type() == second.getButton().type() && second.getButton().type() == type && first.getButton().action() == Button::Action::Released && second.getButton().action() == Button::Action::Pressed && action == Button::Action::Pressed) action = Term::Button::Action::DoubleClicked;
    second              = first;
    first               = Term::Mouse(Term::Button(type, action), values[1], values[2]);
    m_container.m_Mouse = first;
    old                 = std::chrono::system_clock::now();
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
    if(str == "\033OA" || str == "\033[A" || str == "\033A") m_container.m_Key = Key(Term::Key::ArrowUp);
    else if(str == "\033OB" || str == "\033[B" || str == "\033B")
      m_container.m_Key = Key(Term::Key::ArrowDown);
    else if(str == "\033OC" || str == "\033[C" || str == "\033C")
      m_container.m_Key = Key(Term::Key::ArrowRight);
    else if(str == "\033OD" || str == "\033[D" || str == "\033D")
      m_container.m_Key = Key(Term::Key::ArrowLeft);
    /*
     * Key        Normal     Application
     * ---------+----------+-------------
     * Home     | CSI H    | SS3 H
     * End      | CSI F    | SS3 F
     * ---------+----------+-------------
    */
    else if(str == "\033OH" || str == "\033[H")
      m_container.m_Key = Key(Term::Key::Home);
    else if(str == "\033OF" || str == "\033[F")
      m_container.m_Key = Key(Term::Key::End);
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
      m_container.m_Key = Key(Term::Key::F1);
    else if(str == "\033OQ" || str == "\033[12~")
      m_container.m_Key = Key(Term::Key::F2);
    else if(str == "\033OR" || str == "\033[13~")
      m_container.m_Key = Key(Term::Key::F3);
    else if(str == "\033OS" || str == "\033[14~")
      m_container.m_Key = Key(Term::Key::F4);
    else if(str == "\033[15~")
      m_container.m_Key = Key(Term::Key::F5);
    else if(str == "\033[17~")
      m_container.m_Key = Key(Term::Key::F6);
    else if(str == "\033[18~")
      m_container.m_Key = Key(Term::Key::F7);
    else if(str == "\033[19~")
      m_container.m_Key = Key(Term::Key::F8);
    else if(str == "\033[20~")
      m_container.m_Key = Key(Term::Key::F9);
    else if(str == "\033[21~")
      m_container.m_Key = Key(Term::Key::F10);
    else if(str == "\033[23~")
      m_container.m_Key = Key(Term::Key::F11);
    else if(str == "\033[24~")
      m_container.m_Key = Key(Term::Key::F12);
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
      m_container.m_Key = Key(Term::Key::Insert);
    else if(str == "\033[3~")
      m_container.m_Key = Key(Term::Key::Del);
    else if(str == "\033[1~")
      m_container.m_Key = Key(Term::Key::Home);
    else if(str == "\033[4~")
      m_container.m_Key = Key(Term::Key::End);
    else if(str == "\033[5~")
      m_container.m_Key = Key(Term::Key::PageUp);
    else if(str == "\033[6~")
      m_container.m_Key = Key(Term::Key::PageDown);
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
      m_container.m_Key = Key(Term::Key::F13);
    else if(str == "\033[26~")
      m_container.m_Key = Key(Term::Key::F14);
    else if(str == "\033[28~")
      m_container.m_Key = Key(Term::Key::F15);
    else if(str == "\033[29~")
      m_container.m_Key = Key(Term::Key::F16);
    else if(str == "\033[31~")
      m_container.m_Key = Key(Term::Key::F17);
    else if(str == "\033[32~")
      m_container.m_Key = Key(Term::Key::F18);
    else if(str == "\033[33~")
      m_container.m_Key = Key(Term::Key::F19);
    else if(str == "\033[34~")
      m_container.m_Key = Key(Term::Key::F20);
    else if(str == "\033[G")
      m_container.m_Key = Key(Term::Key::Value::Numeric5);
    else if(Term::Private::is_valid_utf8_code_unit(str))
      m_container.m_Key = Key(static_cast<Term::Key::Value>(Term::Private::utf8_to_utf32(str)[0]));
    else
    {
      m_Type = Type::CopyPaste;
      new(&this->m_container.m_string) std::string(str);
      return;
    }
    m_Type = Type::Key;
  }
  else
  {
    m_Type = Type::CopyPaste;
    new(&this->m_container.m_string) std::string(str);
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

Term::Event::operator Term::Focus() const
{
  if(m_Type == Type::Focus) return m_container.m_Focus;
  else
    return Focus();
}
