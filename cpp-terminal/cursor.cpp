#include "cpp-terminal/cursor.hpp"

// C++ libs
#include <cstdlib> // for integer to string conversions 

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

void Term::cursor_off(std::string& stringOut) { stringOut.append("\x1b[?25l"); }

std::string Term::cursor_off() { return std::string("\x1b[?25l"); }

void Term::cursor_on(std::string& stringOut) { stringOut.append("\x1b[?25h"); }

std::string Term::cursor_on() { return std::string("\x1b[?25h"); }

void Term::cursor_move(std::string stringOut&, std::size_t row, std::size_t column) { 	
	stringOut.append("\x1b[");
	stringOut.append(std::to_string(row));
	stringOut.append(';');
	stringOut.append(std::to_string(column));
	stringOut.append('H');
 }

std::string Term::cursor_move(std::size_t row, std::size_t column) { return "\x1b[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }


std::string Term::cursor_up(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'A'; }

void Term::cursor_up(std::string& stringOut, std::size_t rows) { 
	stringOut.append("\x1b[");
	stringOut.append(std::to_string(rows));
	stringOut.append('A');
}

std::string Term::cursor_down(std::size_t rows) { return "\x1b[" + std::to_string(rows) + 'B'; }

void Term::cursor_down(std::string& stringOut, std::size_t rows) { 
	stringOut.append("\x1b[");
	stringOut.append(std::to_string(rows));
	stringOut.append('B');
}

std::string Term::cursor_right(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'C'; }

void Term::cursor_right(std::string& stringOut, std::size_t columns) { 
	stringOut.append("\x1b[");
	stringOut.append(std::to_string(columns));
	stringOut.append('C');
	
}

std::string Term::cursor_left(std::size_t columns) { return "\x1b[" + std::to_string(columns) + 'D'; }

void Term::cursor_left(std::string& stringOut, std::size_t columns) { 
	stringOut.append("\x1b[");
	stringOut.append(std::to_string(columns));
	stringOut.append('D');
}

std::string Term::cursor_position_report() { return "\x1b[6n"; }

void Term::cursor_position_report(std::string& stringOut, ) { 
	stringOut.append("\x1b[6n"); 
}

std::string Term::clear_eol() { return "\x1b[K"; }

void Term::clear_eol(std::string& stringOut, ) { 
	stringOut("\x1b[K");
}


