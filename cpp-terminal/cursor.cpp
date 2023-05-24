#include "cpp-terminal/cursor.hpp"

Term::Cursor::Cursor(const std::size_t& row, const std::size_t& column) : m_position({row, column}) {}

std::size_t Term::Cursor::row() const { return m_position.first; }

std::size_t Term::Cursor::column() const { return m_position.second; }

bool Term::Cursor::empty() const
{
  if(m_position.first == 0 && m_position.second == 0) return true;
  else
    return false;
}

void Term::Cursor::setRow(const std::size_t& row) { m_position.first = row; }

void Term::Cursor::setColum(const std::size_t& column) { m_position.second = column; }

void Term::cursor_off(std::string& stringOut) { stringOut.append("\x1b[?25l", sizeof("\x1b[?25l")-1); }

std::string Term::cursor_off() { return std::string("\x1b[?25l", sizeof("\x1b[?25l")-1); }

void Term::cursor_on(std::string& stringOut) { stringOut.append("\x1b[?25h", sizeof("\x1b[?25h")-1); }

std::string Term::cursor_on() { return std::string("\x1b[?25h", sizeof("\x1b[?25h")-1); }

void Term::cursor_move(std::string& stringOut, std::size_t row, std::size_t column)
{
  stringOut.append("\x1b[", sizeof("\x1b[")-1);
  stringOut += std::to_string(row);
  stringOut += ';';
  stringOut += std::to_string(column);
  stringOut += 'H';
}

std::string Term::cursor_move(std::size_t row, std::size_t column) { return "\x1b[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }

std::string Term::cursor_up(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'A'; }

void Term::cursor_up(std::string& stringOut, std::size_t rows)
{
  stringOut.append("\x1b[", sizeof("\x1b[")-1);
  stringOut += std::to_string(rows);
  stringOut += 'A';
}

std::string Term::cursor_down(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'B'; }

void Term::cursor_down(std::string& stringOut, std::size_t rows)
{
  stringOut.append("\x1b[", sizeof("\x1b[")-1);
  stringOut += std::to_string(rows);
  stringOut += 'B';
}

std::string Term::cursor_right(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'C'; }

void Term::cursor_right(std::string& stringOut, std::size_t columns)
{
  stringOut.append("\x1b[", sizeof("\x1b[")-1);
  stringOut += std::to_string(columns);
  stringOut += 'C';
}

std::string Term::cursor_left(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'D'; }

void Term::cursor_left(std::string& stringOut, std::size_t columns)
{
  stringOut.append("\x1b[", sizeof("\x1b[")-1);
  stringOut += std::to_string(columns);
  stringOut += 'D';
}

std::string Term::cursor_position_report() { return "\x1b[6n"; }

void Term::cursor_position_report(std::string& stringOut) { stringOut.append("\x1b[6n", sizeof("\x1b[6n")-1); }

std::string Term::clear_eol() { return std::string("\x1b[K", sizeof("\x1b[K")-1); }

void Term::clear_eol(std::string& stringOut) { stringOut.append("\x1b[K", sizeof("\x1b[K")-1); }
