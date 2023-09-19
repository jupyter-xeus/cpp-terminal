#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Term
{

class Cursor
{
public:
  Cursor() = default;
  Cursor(const std::uint16_t&, const std::uint16_t&);
  std::size_t row() const;
  std::size_t column() const;
  void        setRow(const std::uint16_t&);
  void        setColum(const std::uint16_t&);
  bool        empty() const;

private:
  std::pair<std::uint16_t, std::uint16_t> m_position;
};

// returns the current cursor position (row, column) (Y, X)
Term::Cursor cursor_position();

// move the cursor to the given (row, column) / (Y, X)
std::string cursor_move(std::uint16_t row, std::uint16_t column);
// move the cursor the given rows up
std::string cursor_up(std::size_t rows);
// move the cursor the given rows down
std::string cursor_down(std::size_t rows);
// move the cursor the given columns left
std::string cursor_left(std::size_t columns);
// move the cursor the given columns right
std::string cursor_right(std::size_t columns);
// the ANSI code to generate a cursor position report
std::string cursor_position_report();
// turn off the cursor
std::string cursor_off();
// turn on the cursor
std::string cursor_on();

// clears the screen from the current cursor position to the end of the screen
std::string clear_eol();

}  // namespace Term
