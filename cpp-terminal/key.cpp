#include "cpp-terminal/key.hpp"

Term::Key::Key(const Term::Key::Value& value) : m_value(value) {}

Term::Key::operator Term::Key::Value() { return m_value; }

bool Term::Key::is_ASCII()
{
  if(m_value >= 0 && m_value <= 127) return true;
  else
    return false;
}

bool Term::Key::is_extended_ASCII()
{
  if(m_value >= 0 && m_value <= 255) return true;
  else
    return false;
}

bool Term::Key::is_CTRL()
{
  // Need to suppress the TAB etc...
  if(m_value > 0 && m_value <= 31 && m_value != Key::BACKSPACE && m_value != Key::TAB && m_value != ESC && /* the two mapped to ENTER */ m_value != ENTER) return true;
  else
    return false;
}

bool Term::Key::is_ALT()
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
