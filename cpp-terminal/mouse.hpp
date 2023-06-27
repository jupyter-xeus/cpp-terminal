#pragma once

#include <cstddef>
#include <cstdint>

namespace Term
{

class Buttons
{
public:
  enum class Type : std::uint32_t
  {
    None    = 0,
    Left    = 1 << 0,
    Wheel   = 1 << 1,
    Right   = 1 << 2,
    Button1 = 1 << 3,
    Button2 = 1 << 4,
    Button3 = 1 << 5,
    Button4 = 1 << 6,
    Button5 = 1 << 7,
    Button6 = 1 << 8,
    Button7 = 1 << 9,
    Button8 = 1 << 10,
  };
  enum class Action : std::uint32_t
  {
    Clicked,
    Released,
    DoubleClicked,
    RolledUp,
    RolledDown,
    ToRight,
    ToLeft,
    None,
  };
  Buttons() = default;
  Action getAction();
  Type   getType();
  Buttons(const Term::Buttons::Type& type, const Term::Buttons::Action& action, const std::size_t& time);

private:
  std::size_t           m_times{0};
  Term::Buttons::Type   m_type{Term::Buttons::Type::None};
  Term::Buttons::Action m_action{Term::Buttons::Action::None};
};

class Mouse
{
public:
  Mouse() = default;
  Mouse(const Buttons& buttons, const std::size_t& row, const std::size_t& column);
  std::size_t   row() const;
  std::size_t   column() const;
  Term::Buttons getButtons();

private:
  std::size_t   m_rows{0};
  std::size_t   m_columns{0};
  Term::Buttons m_buttons;
};

}  // namespace Term
