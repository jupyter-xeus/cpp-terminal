/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/mouse.hpp"

Term::Button::Action Term::Button::action() const noexcept { return m_action; }

Term::Button::Type Term::Button::type() const noexcept { return m_type; }

bool Term::Button::operator==(const Term::Button& button) const { return (m_action == button.m_action) && (m_type == button.m_type); }

bool Term::Button::operator!=(const Term::Button& button) const { return !(*this == button); }

bool Term::Mouse::is(const Term::Button::Type& type, const Term::Button::Action& action) const noexcept { return (m_buttons.type() == type) && (m_buttons.action() == action); }

bool Term::Mouse::is(const Term::Button::Type& type) const noexcept { return m_buttons.type() == type; }

Term::Button Term::Mouse::getButton() const noexcept { return m_buttons; }

std::size_t Term::Mouse::row() const noexcept { return static_cast<std::size_t>(m_row); }

std::size_t Term::Mouse::column() const noexcept { return static_cast<std::size_t>(m_column); }

bool Term::Mouse::operator==(const Term::Mouse& mouse) const { return (m_row == mouse.m_row) && (m_column == mouse.m_column) && (m_buttons == mouse.m_buttons); }
bool Term::Mouse::operator!=(const Term::Mouse& mouse) const { return !(*this == mouse); }
