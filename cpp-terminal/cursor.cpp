/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/cursor.hpp"

Term::Cursor::Cursor(const std::size_t& row, const std::size_t& column) : m_position({row, column}) {}

std::size_t Term::Cursor::row() const { return m_position.first; }

std::size_t Term::Cursor::column() const { return m_position.second; }

bool Term::Cursor::empty() const { return (0 == m_position.first) && (0 == m_position.second); }

void Term::Cursor::setRow(const std::size_t& row) { m_position.first = row; }

void Term::Cursor::setColum(const std::size_t& column) { m_position.second = column; }

bool Term::Cursor::operator==(const Term::Cursor& cursor) const { return (this->row() == cursor.row()) && (this->column() == cursor.column()); }

bool Term::Cursor::operator!=(const Term::Cursor& cursor) const { return !(*this == cursor); }

std::string Term::cursor_off() { return "\u001b[?25l"; }

std::string Term::cursor_on() { return "\u001b[?25h"; }

std::string Term::cursor_move(const std::size_t& row, const std::size_t& column) { return "\u001b[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }

std::string Term::cursor_up(const std::size_t& rows) { return "\u001b[" + std::to_string(rows) + 'A'; }

std::string Term::cursor_down(const std::size_t& rows) { return "\u001b[" + std::to_string(rows) + 'B'; }

std::string Term::cursor_right(const std::size_t& columns) { return "\u001b[" + std::to_string(columns) + 'C'; }

std::string Term::cursor_left(const std::size_t& columns) { return "\u001b[" + std::to_string(columns) + 'D'; }

std::string Term::cursor_position_report() { return "\u001b[6n"; }

std::string Term::clear_eol() { return "\u001b[K"; }
