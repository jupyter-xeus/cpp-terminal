#pragma once

#include <cpp-terminal/window.hpp>
#include <functional>

namespace Term {
// This model contains all the information about the state of the prompt in an
// abstract way, irrespective of where or how it is rendered.
struct Model {
    std::string prompt_string;  // The string to show as the prompt
    std::vector<std::string>
        lines;  // The current input string in the prompt as
                // a vector of lines, without '\n' at the end.
    // The current cursor position in the "input" string, starting from (1,1)
    size_t cursor_col{}, cursor_row{};
};

std::string concat(const std::vector<std::string>&);

std::vector<std::string> split(const std::string&);

char32_t U(const std::string&);

void print_left_curly_bracket(Term::Window&, int, int, int);

void render(Term::Window&, const Model&, size_t);

std::string prompt(Terminal&,
                   const std::string&,
                   std::vector<std::string>&,
                   std::function<bool(std::string)>&);
}  // namespace Term
