#include "cpp-terminal/cursor.hpp"

Term::Cursor::Cursor(const std::size_t& row, const std::size_t& column) : m_rows(row), m_columns(column) {}

std::size_t Term::Cursor::row() const { return m_rows; }

std::size_t Term::Cursor::column() const { return m_columns; }

bool Term::Cursor::empty() const
{
  if(m_rows == 0 && m_columns == 0) return true;
  else
    return false;
}

void Term::Cursor::setRow(const std::size_t& row) { m_rows = row; }

void Term::Cursor::setColum(const std::size_t& column) { m_columns = column; }

std::string Term::cursor_off() { return "\x1b[?25l"; }

std::string Term::cursor_on() { return "\x1b[?25h"; }

std::string Term::cursor_move(std::size_t row, std::size_t column) { return "\x1b[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }

std::string Term::cursor_up(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'A'; }

std::string Term::cursor_down(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'B'; }

std::string Term::cursor_right(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'C'; }

std::string Term::cursor_left(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'D'; }

std::string Term::cursor_position_report() { return "\x1b[6n"; }

std::string Term::clear_eol() { return "\x1b[K"; }
