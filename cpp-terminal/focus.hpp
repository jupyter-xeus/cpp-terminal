/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>

namespace Term
{

/**
 * @brief Class to return the focus of the terminal
 *
 */
class Focus
{
public:
  enum class Type : std::int8_t
  {
    Unknown = -1,  ///< The terminal focus is \b unknown.
    Out     = 0,   ///< The terminal focus is \b out.
    In      = 1,   ///< The terminal focus is \b in.
  };

  Focus() = default;

  explicit Focus(const Term::Focus::Type& type);

  /**
   * @brief Get the type of focus.
   *
   * @return Term::Focus::Type
   */
  Term::Focus::Type type() const;

  /**
   * @brief Check is the focus is \b in.
   *
   * @return true : The terminal has focus \b in.
   * @return false : The terminal has focus \b out.
   */
  bool in() const;

  /**
   * @brief Check is the focus is \b out.
   *
   * @return true : The terminal has focus \b out.
   * @return false : The terminal has focus \b in.
   */
  bool out() const;

  bool operator==(const Term::Focus& focus) const;
  bool operator!=(const Term::Focus& focus) const;

private:
  Term::Focus::Type m_focus{Term::Focus::Type::Unknown};
};

}  // namespace Term
