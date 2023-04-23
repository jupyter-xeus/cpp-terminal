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

std::string Term::clear_screen() { return "\x1b[2J"; }

std::string Term::screen_save()
{
  return "\0337\033[?1049h";  // save current cursor position, save screen
}

std::string Term::screen_load()
{
  return "\033[?1049l\0338";  // restores screen, restore current cursor position
}
