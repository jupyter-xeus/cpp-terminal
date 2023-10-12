/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace Term
{

class Button
{
public:
  enum class Type : std::int8_t
  {
    None = -1,
    Left,
    Wheel,
    Right,
    Button1,
    Button2,
    Button3,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
  };
  enum class Action : std::int8_t
  {
    None = 0,
    Pressed,
    Released,
    DoubleClicked,
    RolledUp,
    RolledDown,
    ToRight,
    ToLeft,
  };
  Button() = default;
  Button(const Term::Button::Type& type, const Term::Button::Action& action) : m_type(type), m_action(action) {}
  Term::Button::Action action();
  Term::Button::Type   type();
  bool                 operator==(const Term::Button& button) const;
  bool                 operator!=(const Term::Button& button) const;

private:
  Term::Button::Type   m_type{Term::Button::Type::None};
  Term::Button::Action m_action{Term::Button::Action::None};
};

class Mouse
{
public:
  Mouse() = default;
  Mouse(const Term::Button& buttons, const std::uint16_t& row, const std::uint16_t& column) : m_buttons(buttons), m_row(row), m_column(column) {}
  std::size_t  row();
  std::size_t  column();
  Term::Button getButton();
  bool         is(const Term::Button::Type& type, const Term::Button::Action& action);
  bool         is(const Term::Button::Type& type);
  bool         operator==(const Term::Mouse&) const;
  bool         operator!=(const Term::Mouse&) const;

private:
  Term::Button  m_buttons;
  std::uint16_t m_row{0};
  std::uint16_t m_column{0};
};

}  // namespace Term
