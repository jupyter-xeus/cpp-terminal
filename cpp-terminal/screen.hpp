
#pragma once

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

}  // namespace Term