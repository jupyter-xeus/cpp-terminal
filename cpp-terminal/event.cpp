#include "cpp-terminal/event.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

Term::Event Term::parse_event(std::string&& str)
{
  if(str.size() == 1)
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
