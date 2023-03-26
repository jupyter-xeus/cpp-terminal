#include "cpp-terminal/screen.hpp"

std::size_t Term::Screen::rows() { return m_size.first; }

std::size_t Term::Screen::columns() { return m_size.second; }

bool Term::Screen::empty()
{
  if(m_size.second == 0 && m_size.first == 0) return true;
  else
    return false;
}

std::pair<std::size_t, std::size_t> Term::Screen::size() { return m_size; }