#include "cpp-terminal/key.hpp"
#include "cpp-terminal/platforms/conversion.hpp"

Term::MetaKey::MetaKey(const MetaKey::Value& value) : m_value(value){}

Term::MetaKey Term::MetaKey::operator+(const Term::MetaKey& meta) const
{
  return MetaKey(static_cast<MetaKey::Value>(m_value|meta.m_value));
}

bool Term::MetaKey::hasAlt() const
{
  if((m_value&Value::Alt)==Value::Alt) return true;
  else return false;
}

bool Term::MetaKey::hasCtrl() const
{
  if((m_value&Value::Ctrl)==Value::Ctrl) return true;
  else return false;
}

Term::Key     Term::MetaKey::operator+(const Term::Key& key) const
{
  if(key.empty()) return key;
  switch(m_value)
  {
    case None:
      return key;
    case Ctrl:
      if((m_value==Value::Ctrl)&&key.hasCtrlAll()) return key;
      else return Key(static_cast<Term::Key::Value>(key+Value::Ctrl)); // FIXME maybe a better check;
    case Alt:
      if((m_value==Value::Alt)&&key.hasAlt()) return key;
      else return Key(static_cast<Term::Key::Value>(key+Value::Alt)); // FIXME maybe a better check;
  }
}

bool          Term::MetaKey::operator==(const Term::MetaKey& meta) const
{
  if(meta.m_value==m_value) return true;
  else return false;
}

bool          Term::MetaKey::operator!=(const Term::MetaKey& meta) const
{
  return !(meta==*this);
}

Term::Key::Key(const Term::Key::Value& value) : m_value(value) {}

Term::Key::operator Term::Key::Value() const { return m_value; }


std::string Term::Key::str() const
{
  std::string ret;
  if(m_value >= 0x10FFFF) return std::string();
  else
  {
    Term::Private::codepoint_to_utf8(ret, static_cast<char32_t>(m_value));
    return ret;
  }
}





bool Term::Key::isASCII() const
{
  if(m_value >= Null && m_value <= Del) return true;
  else
    return false;
}

bool Term::Key::isExtendedASCII() const
{
  if(m_value >= Null && m_value <= 255) return true;
  else
    return false;
}

bool Term::Key::hasCtrl() const
{
  // Need to suppress the TAB etc...
  if((iscntrl() || hasCtrlAll())&& m_value != Key::Backspace && m_value != Key::Tab && m_value != Esc && m_value != Enter && m_value != Del) return true;
  else
    return false;
}

bool Term::Key::hasCtrlAll() const
{
  if((iscntrl()||((m_value&Term::MetaKey::Value::Ctrl)== Term::MetaKey::Value::Ctrl))) return true;
  else return false;
}


bool Term::Key::hasAlt()  const
{
  if((m_value&Term::MetaKey::Value::Alt)== Term::MetaKey::Value::Alt) return true;
  else
    return false;
}

bool Term::Key::empty() const
{
  if(m_value == NoKey) return true;
  else
    return false;
}

bool Term::Key::iscntrl() const
{
  if((m_value >= Null && m_value <= Ctrl_Underscore) || m_value == Del) return true;
  else
    return false;
}

bool Term::Key::isblank() const
{
  if(m_value == Tab || m_value == Space) return true;
  else
    return false;
}

bool Term::Key::isspace() const
{
  if(isblank() || (m_value >= Ctrl_J && m_value <= Enter)) return true;
  else
    return false;
}

bool Term::Key::isupper() const
{
  if(m_value >= A && m_value <= Z) return true;
  else
    return false;
}

bool Term::Key::islower() const
{
  if(m_value >= a && m_value <= z) return true;
  else
    return false;
}

bool Term::Key::isalpha() const
{
  if(isupper() || islower()) return true;
  else
    return false;
}

bool Term::Key::isdigit() const
{
  if(m_value >= Zero && m_value <= Nine) return true;
  else
    return false;
}

bool Term::Key::isxdigit() const
{
  if(isdigit() || (m_value >= A && m_value <= F) || (m_value >= a && m_value <= f)) return true;
  else
    return false;
}

bool Term::Key::isalnum() const
{
  if(isdigit() || isalpha()) return true;
  else
    return false;
}

bool Term::Key::ispunct() const
{
  if((m_value >= ExclamationMark && m_value <= Slash) || (m_value >= Colon && m_value <= Arobase) || (m_value >= OpenBracket && m_value <= GraveAccent) || (m_value >= OpenBrace && m_value <= Tilde)) return true;
  else
    return false;
}

bool Term::Key::isgraph() const
{
  if(isalnum() || ispunct()) return true;
  else
    return false;
}

bool Term::Key::isprint() const
{
  if(isgraph() || m_value == Space) return true;
  else
    return false;
}

char Term::Key::tolower()
{
  if(isalpha() && isupper()) return static_cast<char>(m_value + 32);
  else
    return static_cast<char>(m_value);
}

char Term::Key::toupper()
{
  if(isalpha() && islower()) return static_cast<char>(m_value - 32);
  else
    return static_cast<char>(m_value);
}

bool Term::Key::isunicode() const //FIXME not check for bad unicode
{
  if(m_value>=Null&&m_value<=0x10FFFF) return true;
  else return false;
}

std::string Term::Key::name() const
{
  if(m_value==NoKey) return "";
  Term::Key::Value key=m_value;
  std::string ret;
  if(hasAlt())
  {
    ret+="Alt+";
    key=static_cast<Term::Key::Value>(key-Term::MetaKey::Alt);
  }
  if(hasCtrl())
  {
    ret+="Ctrl+";
    if(!iscntrl()) key=static_cast<Term::Key::Value>(key-Term::MetaKey::Ctrl);
  }
  if(m_value==Tab) ret+="Tab";
  else if(m_value==Enter) ret+="Enter";
  else if(m_value==Esc) ret+="Esc";
  else if(m_value==Backspace) ret+="Backspace";
  else if(m_value==Del) ret+="Del";
  else if(iscntrl()) ret+=static_cast<int>(m_value + 64);
  else if(m_value==Space) ret+="Space";
  else if(Term::Key(key).isunicode())
  {
    if(key!=m_value)ret+=Term::Key(key).name();
    else ret+=str();
  }
  else
  {
    switch(m_value)
    {
      case Term::Key::ArrowLeft: ret += "Left Arrow"; break;
      case Term::Key::ArrowRight: ret += "Right Arrow"; break;
      case Term::Key::ArrowUp: ret += "Up arrow"; break;
      case Term::Key::ArrowDown: ret += "Down arrow"; break;
      case Term::Key::Numeric5: ret += "5 Numeric pad"; break;
      case Term::Key::Home: ret += "Home"; break;
      case Term::Key::Insert: ret += "Insert"; break;
      case Term::Key::End: ret += "End"; break;
      case Term::Key::PageUp: ret += "Page up"; break;
      case Term::Key::PageDown: ret += "Page down"; break;
      case Term::Key::F1: ret += "F1"; break;
      case Term::Key::F2: ret += "F2"; break;
      case Term::Key::F3: ret += "F3"; break;
      case Term::Key::F4: ret += "F4"; break;
      case Term::Key::F5: ret += "F5"; break;
      case Term::Key::F6: ret += "F6"; break;
      case Term::Key::F7: ret += "F7"; break;
      case Term::Key::F8: ret += "F8"; break;
      case Term::Key::F9: ret += "F9"; break;
      case Term::Key::F10: ret += "F10"; break;
      case Term::Key::F11: ret += "F11"; break;
      case Term::Key::F12: ret += "F12"; break;
      case Term::Key::F13: ret += "F13"; break;
      case Term::Key::F14: ret += "F14"; break;
      case Term::Key::F15: ret += "F15"; break;
      case Term::Key::F16: ret += "F16"; break;
      case Term::Key::F17: ret += "F17"; break;
      case Term::Key::F18: ret += "F18"; break;
      case Term::Key::F19: ret += "F19"; break;
      case Term::Key::F20: ret += "F20"; break;
      case Term::Key::F21: ret += "F21"; break;
      case Term::Key::F22: ret += "F22"; break;
      case Term::Key::F23: ret += "F23"; break;
      case Term::Key::F24: ret += "F24"; break;
      case Term::Key::PrintScreen: ret+= "Print Screen";break;
      case Term::Key::Menu: ret+="Menu";break;
    }
  }
  return ret;
}

