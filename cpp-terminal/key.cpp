#include "cpp-terminal/key.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

char32_t Term::Key::codepoint() { return static_cast<char32_t>(m_value); }

std::string Term::Key::str()
{
  std::string ret;
  Term::Private::codepoint_to_utf8(ret, codepoint());
  return ret;
}

Term::Key::Key(const Term::Key::Value& value) : m_value(value) {}

Term::Key::operator Term::Key::Value() { return m_value; }

bool Term::Key::isASCII()
{
  if(m_value >= NUL && m_value <= DEL) return true;
  else
    return false;
}

bool Term::Key::isExtendedASCII()
{
  if(m_value >= NUL && m_value <= 255) return true;
  else
    return false;
}

bool Term::Key::isCTRL()
{
  // Need to suppress the TAB etc...
  if(iscntrl() && m_value != Key::BACKSPACE && m_value != Key::TAB && m_value != ESC && m_value != ENTER && m_value != DEL) return true;
  else
    return false;
}

bool Term::Key::isALT()
{
  if(m_value >= Key::ALT && Key::ALT < (Key::ALT << 1)) return true;
  else
    return false;
}

bool Term::Key::empty()
{
  if(m_value == NO_KEY) return true;
  else
    return false;
}

bool Term::Key::iscntrl()
{
  if((m_value >= NUL && m_value <= CTRL_UNDERSCORE) || m_value == DEL) return true;
  else
    return false;
}

bool Term::Key::isblank()
{
  if(m_value == TAB || m_value == SPACE) return true;
  else
    return false;
}

bool Term::Key::isspace()
{
  if(isblank() || (m_value >= CTRL_J && m_value <= ENTER)) return true;
  else
    return false;
}

bool Term::Key::isupper()
{
  if(m_value >= A && m_value <= Z) return true;
  else
    return false;
}

bool Term::Key::islower()
{
  if(m_value >= a && m_value <= z) return true;
  else
    return false;
}

bool Term::Key::isalpha()
{
  if(isupper() || islower()) return true;
  else
    return false;
}

bool Term::Key::isdigit()
{
  if(m_value >= ZERO && m_value <= NINE) return true;
  else
    return false;
}

bool Term::Key::isxdigit()
{
  if(isdigit() || (m_value >= A && m_value <= F) || (m_value >= a && m_value <= f)) return true;
  else
    return false;
}

bool Term::Key::isalnum()
{
  if(isdigit() || isalpha()) return true;
  else
    return false;
}

bool Term::Key::ispunct()
{
  if((m_value >= EXCLAMATION_MARK && m_value <= SLASH) || (m_value >= COLON && m_value <= AROBASE) || (m_value >= OPEN_BRACKET && m_value <= GRAVE_ACCENT) || (m_value >= OPEN_BRACE && m_value <= TILDE)) return true;
  else
    return false;
}

bool Term::Key::isgraph()
{
  if(isalnum() || ispunct()) return true;
  else
    return false;
}

bool Term::Key::isprint()
{
  if(isgraph() || m_value == SPACE) return true;
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
