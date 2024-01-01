/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/focus.hpp"

namespace Term
{

Focus::Focus(const Term::Focus::Type& type) : m_focus(type) {}

Term::Focus::Type Focus::type() const { return m_focus; }

bool Focus::in() const { return m_focus == Term::Focus::Type::In; }

bool Focus::out() const { return m_focus == Term::Focus::Type::Out; }

bool Term::Focus::operator==(const Term::Focus& focus) const { return m_focus == focus.m_focus; }

bool Term::Focus::operator!=(const Term::Focus& focus) const { return !(*this == focus); }

}  // namespace Term
