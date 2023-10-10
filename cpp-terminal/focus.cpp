/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

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

bool Term::Focus::operator==(const Term::Focus& focus) const
{
  if(m_focus == focus.m_focus) return true;
  else
    return false;
}

bool Term::Focus::operator!=(const Term::Focus& focus) const { return !(*this == focus); }

}  // namespace Term
