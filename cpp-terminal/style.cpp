#include "cpp-terminal/style.hpp"

std::string Term::style(const Term::Style& style) { return "\033[" + std::to_string((std::uint8_t)style) + 'm'; }
