/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
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
  Term::Button::Action action() const noexcept;
  Term::Button::Type   type() const noexcept;
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
  std::size_t  row() const noexcept;
  std::size_t  column() const noexcept;
  Term::Button getButton() const noexcept;
  bool         is(const Term::Button::Type& type, const Term::Button::Action& action) const noexcept;
  bool         is(const Term::Button::Type& type) const noexcept;
  bool         operator==(const Term::Mouse& mouse) const;
  bool         operator!=(const Term::Mouse& mouse) const;

private:
  Term::Button  m_buttons;
  std::uint16_t m_row{0};
  std::uint16_t m_column{0};
};

}  // namespace Term
