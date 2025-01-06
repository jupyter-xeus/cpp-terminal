/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace Term
{

class Rows
{
public:
  Rows() = default;
  explicit Rows(const std::uint16_t& rows) : m_rows(rows) {}
  operator std::size_t() const noexcept { return m_rows; }

private:
  std::uint16_t m_rows{0};
};

class Columns
{
public:
  Columns() = default;
  explicit Columns(const std::uint16_t& columns) : m_columns(columns) {}
  operator std::size_t() const noexcept { return m_columns; }

private:
  std::uint16_t m_columns{0};
};

class Size
{
public:
  Size() = default;
  Size(const Rows& rows, const Columns& columns) : m_rows(rows), m_columns(columns) {};
  Size(const Columns& columns, const Rows& rows) : m_rows(rows), m_columns(columns) {};
  std::size_t    area() const noexcept { return static_cast<std::size_t>(m_rows) * static_cast<std::size_t>(m_columns); }
  const Rows&    rows() const noexcept { return m_rows; }
  const Columns& columns() const noexcept { return m_columns; }

private:
  Rows    m_rows;
  Columns m_columns;
};

}  // namespace Term
