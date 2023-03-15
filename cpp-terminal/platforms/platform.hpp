#pragma once

#include <string>
#include <utility>

namespace Term
{
namespace Private
{

// returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
// if the console is not connected
std::pair<std::size_t, std::size_t> get_term_size();

}  // namespace Private
}  // namespace Term
