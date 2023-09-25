#include "cpp-terminal/mouse.hpp"

Term::Button::Action Term::Button::action() { return m_action; }

Term::Button::Type Term::Button::type() { return m_type; }

bool Term::Mouse::has(const Term::Button::Type& type, const Term::Button::Action& action) { return m_buttons[static_cast<std::size_t>(type)].action() == action; }

std::array<Term::Button, 11> Term::Mouse::getButtons() const { return m_buttons; }

bool Term::Mouse::has(const Term::Button::Type& type) { return m_buttons[static_cast<std::size_t>(type)].action() != Term::Button::Action::None; }

std::size_t Term::Mouse::row() { return static_cast<std::size_t>(m_row); }

std::size_t Term::Mouse::column() { return static_cast<std::size_t>(m_column); }
