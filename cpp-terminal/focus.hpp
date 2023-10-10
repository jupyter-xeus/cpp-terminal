/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>

namespace Term
{

class Focus
{
public:
  enum class Type : std::int8_t
  {
    Out     = 0,
    In      = 1,
    Unknown = 2,
  };
  Focus() = default;
  explicit Focus(const Term::Focus::Type& type);
  Term::Focus::Type type();
  bool              isIn();
  bool              isOut();
  bool              operator==(const Term::Focus& focus) const;
  bool              operator!=(const Term::Focus& focus) const;

private:
  Term::Focus::Type m_focus{Term::Focus::Type::Unknown};
};

}  // namespace Term
