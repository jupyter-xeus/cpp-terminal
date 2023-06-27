#include "cpp-terminal/mouse.hpp"

namespace Term
{

Buttons::Buttons(const Term::Buttons::Type& type, const Term::Buttons::Action& action, const std::size_t& time) : m_type(type), m_action(action), m_times(time)
{

}

Mouse::Mouse(const Buttons& buttons, const std::size_t& row, const std::size_t& column) : m_buttons(buttons), m_rows(row), m_columns(column)
{

}

Term::Buttons Mouse::getButtons()
{
  return m_buttons;
}

Term::Buttons::Type Buttons::getType()
{
  return m_type;
}

Term::Buttons::Action Buttons::getAction()
{
  return m_action;
}

std::size_t Mouse::row() const
{
  return m_rows;
}

std::size_t Mouse::column() const
{
  return m_columns;
}

}