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

class Row
{
public:
  Row() = default;
  explicit Row(const std::uint16_t& row) : m_row(row) {}
  operator std::size_t() const noexcept { return m_row; }

private:
  std::uint16_t m_row{0};
};

class Column
{
public:
  Column() = default;
  explicit Column(const std::uint16_t& column) : m_column(column) {}
  operator std::size_t() const noexcept { return m_column; }

private:
  std::uint16_t m_column{0};
};

class Position
{
public:
  Position() = default;
  Position(const Row& row, const Column& column) : m_row(row), m_column(column) {};
  Position(const Column& column, const Row& row) : m_row(row), m_column(column) {};
  const Row&    row() const noexcept { return m_row; }
  const Column& column() const noexcept { return m_column; }

private:
  Row    m_row{1};
  Column m_column{1};
};

}  // namespace Term
