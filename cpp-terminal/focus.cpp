#include "cpp-terminal/focus.hpp"

namespace Term
{

Focus::Focus(const Term::Focus::Type& type) { m_focus = type; }

Term::Focus::Type Focus::type() { return m_focus; }

bool Focus::isIn()
{
  if(m_focus == Term::Focus::Type::In) return true;
  else
    return false;
}

bool Focus::isOut()
{
  if(m_focus == Term::Focus::Type::Out) return true;
  else
    return false;
}

}  // namespace Term
