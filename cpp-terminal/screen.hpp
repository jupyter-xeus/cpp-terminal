#pragma once

#include <string>
#include <utility>

namespace Term
{

class Screen
{
public:
  Screen() = default;
  Screen(const std::size_t& rows, const std::size_t& columns) { m_size = {rows, columns}; }
  std::size_t                         rows();
  std::size_t                         columns();
  std::pair<std::size_t, std::size_t> size();
  bool                                empty();

private:
  std::pair<std::size_t, std::size_t> m_size{0, 0};
};

// clear the screen
std::string clear_screen();
// save the current terminal state
std::string screen_save();
// load a previously saved terminal state
std::string screen_load();
// get the terminal size (row, column) / (Y, X)
Screen      screen_size();

}  // namespace Term
