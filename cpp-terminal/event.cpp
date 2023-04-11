#include "cpp-terminal/event.hpp"

bool Term::Event::empty()
{
  if(m_Type == Type::Empty) return true;
  else
    return false;
}

Term::Event::operator std::string()
{
  if(m_Type == Type::CopyPaste) return m_str;
  else
    return std::string();
}

Term::Event::operator Term::Screen()
{
  if(m_Type == Type::Screen) return m_Screen;
  else
    return Term::Screen();
}

Term::Event::Event(const Term::Screen& screen) : m_Type(Type::Screen), m_Screen(screen) {}

Term::Event::Event(const Term::Key& key) : m_Type(Type::Key), m_Key(key) {}

Term::Event::Type Term::Event::type() { return m_Type; }

Term::Event::Event(const std::string& str) : m_Type(Type::CopyPaste), m_str(str) { parse(); }

void Term::Event::parse()
{
  if(m_str.size() == 1)
  {
    m_Type = Type::Key;
    m_Key  = static_cast<Term::Key::Value>(m_str[0]);
    /* Backspace return 127 CTRL+backspace return 8 */
    if(m_Key == Term::Key::Value::DEL) m_Key = Term::Key::Value::BACKSPACE;
    m_str.clear();
  }
  else if(m_str.size() == 2 && m_str[0] == '\033')
  {
    m_Key  = static_cast<Term::Key::Value>(Term::Key::Value::ALT + static_cast<Term::Key::Value>(m_str[1]));
    m_Type = Type::Key;
    m_str.clear();
  }
  else if(m_str[0] == '\033' && m_str[1] == '[' && m_str[m_str.size() - 1] == 'R')
  {
    std::size_t found = m_str.find(';', 2);
    if(found != std::string::npos)
    {
      m_Type = Type::Cursor;

      m_Cursor = Cursor(std::stoi(m_str.substr(2, found - 2)), std::stoi(m_str.substr(found + 1, m_str.size() - (found + 2))));
    }
  }
  else if(m_str.size() <= 10)
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
    if(m_str == "\033OA" || m_str == "\033[A" || m_str == "\033A") m_Key = Term::Key::Value::ARROW_UP;
    else if(m_str == "\033OB" || m_str == "\033[B" || m_str == "\033B")
      m_Key = Term::Key::Value::ARROW_DOWN;
    else if(m_str == "\033OC" || m_str == "\033[C" || m_str == "\033C")
      m_Key = Term::Key::Value::ARROW_RIGHT;
    else if(m_str == "\033OD" || m_str == "\033[D" || m_str == "\033D")
      m_Key = Term::Key::Value::ARROW_LEFT;
    /*
     * Key        Normal     Application
     * ---------+----------+-------------
     * Home     | CSI H    | SS3 H
     * End      | CSI F    | SS3 F
     * ---------+----------+-------------
    */
    else if(m_str == "\033OH" || m_str == "\033[H")
      m_Key = Term::Key::Value::HOME;
    else if(m_str == "\033OF" || m_str == "\033[F")
      m_Key = Term::Key::Value::END;
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
    else if(m_str == "\033OP" || m_str == "\033[11~")
      m_Key = Term::Key::Value::F1;
    else if(m_str == "\033OQ" || m_str == "\033[12~")
      m_Key = Term::Key::Value::F2;
    else if(m_str == "\033OR" || m_str == "\033[13~")
      m_Key = Term::Key::Value::F3;
    else if(m_str == "\033OS" || m_str == "\033[14~")
      m_Key = Term::Key::Value::F4;
    else if(m_str == "\033[15~")
      m_Key = Term::Key::Value::F5;
    else if(m_str == "\033[17~")
      m_Key = Term::Key::Value::F6;
    else if(m_str == "\033[18~")
      m_Key = Term::Key::Value::F7;
    else if(m_str == "\033[19~")
      m_Key = Term::Key::Value::F8;
    else if(m_str == "\033[20~")
      m_Key = Term::Key::Value::F9;
    else if(m_str == "\033[21~")
      m_Key = Term::Key::Value::F10;
    else if(m_str == "\033[23~")
      m_Key = Term::Key::Value::F11;
    else if(m_str == "\033[24~")
      m_Key = Term::Key::Value::F12;
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
    else if(m_str == "\033[2~")
      m_Key = Term::Key::Value::INSERT;
    else if(m_str == "\033[3~")
      m_Key = Term::Key::Value::DEL;
    else if(m_str == "\033[1~")
      m_Key = Term::Key::Value::HOME;
    else if(m_str == "\033[4~")
      m_Key = Term::Key::Value::END;
    else if(m_str == "\033[5~")
      m_Key = Term::Key::Value::PAGE_UP;
    else if(m_str == "\033[6~")
      m_Key = Term::Key::Value::PAGE_DOWN;
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
    else if(m_str == "\033[25~")
      m_Key = Term::Key::Value::F13;
    else if(m_str == "\033[26~")
      m_Key = Term::Key::Value::F14;
    else if(m_str == "\033[28~")
      m_Key = Term::Key::Value::F15;
    else if(m_str == "\033[29~")
      m_Key = Term::Key::Value::F16;
    else if(m_str == "\033[31~")
      m_Key = Term::Key::Value::F17;
    else if(m_str == "\033[32~")
      m_Key = Term::Key::Value::F18;
    else if(m_str == "\033[33~")
      m_Key = Term::Key::Value::F19;
    else if(m_str == "\033[34~")
      m_Key = Term::Key::Value::F20;
    if(!m_Key.empty())
    {
      m_Type = Type::Key;
      m_str.clear();
    }
  }
  else { m_Type = Type::CopyPaste; }
}

Term::Event::operator Term::Key()
{
  if(m_Type == Type::Key) return m_Key;
  else
    return Key();
}

Term::Event::operator Term::Cursor()
{
  if(m_Type == Type::Cursor) return m_Cursor;
  else
    return Cursor();
}
