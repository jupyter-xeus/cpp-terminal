#pragma once

#include <cstddef>
#include <string>

namespace Term
{

class Cursor
{
public:
  Cursor() = default;
  Cursor(const std::size_t& row, const std::size_t& column);
  std::size_t row() const;
  std::size_t column() const;
  void        setRow(const std::size_t&);
  void        setColum(const std::size_t&);
  bool        empty() const;

private:
  std::size_t m_rows{0};
  std::size_t m_columns{0};
};

// returns the current cursor position (row, column) (Y, X)
Term::Cursor cursor_position();

// move the cursor to the given (row, column) / (Y, X)
std::string cursor_move(std::size_t row, std::size_t column);
void        cursor_move(std::string& stringOut, std::size_t row, std::size_t column);

// move the cursor the given rows up
std::string cursor_up(std::size_t rows);
void        cursor_up(std::string& stringOut, std::size_t rows);

// move the cursor the given rows down
std::string cursor_down(std::size_t rows);
void        cursor_down(std::string& stringOut, std::size_t rows);

// move the cursor the given columns left
std::string cursor_left(std::size_t columns);
void        cursor_left(std::string& stringOut, std::size_t columns);

// move the cursor the given columns right
std::string cursor_right(std::size_t columns);
void        cursor_right(std::string& stringOut, std::size_t columns);

// the ANSI code to generate a cursor position report
std::string cursor_position_report();
void        cursor_position_report(std::string& stringOut);

// turn off the cursor
std::string cursor_off();
void        cursor_off(std::string& stringOut);

// turn on the cursor
std::string cursor_on();
void        cursor_on(std::string& stringOut);

// clears the screen from the current cursor position to the end of the screen
std::string clear_eol();
void        clear_eol(std::string& stringOut);

}  // namespace Term
