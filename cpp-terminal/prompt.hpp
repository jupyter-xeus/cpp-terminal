#pragma once

#include <cpp-terminal/window.hpp>
#include <functional>

// TODO: remove windows.h include and this undefine
#undef ERROR

namespace Term {
/* Basic prompt */

// indicates the results of prompt_blocking() and prompt_non_blocking
enum class Result {
    YES,
    NO,
    ERROR,
    NONE,
    ABORT,
    INVALID,
};
// indicates the results of prompt_simple()
enum class Result_simple { YES, NO, ABORT };

// A simple yes/no prompt, requires the user to press the ENTER key to continue
// The arguments are used like this: 1 [2/3]4 <user Input>
Result prompt_blocking(std::string message,
                       std::string first_option,
                       std::string second_option,
                       std::string prompt_indicator);

// A simple yes/no prompt, returns immediately after the first key press
// The arguments are used like this: 1 [2/3]4 <user Input>
Result prompt_non_blocking(std::string message,
                           std::string first_option,
                           std::string second_option,
                           std::string prompt_indicator);

// The most simple prompt possible, requires the user to press enter to continue
// The arguments are used like this: 1 [y/N]:
// Invalid input, errors (like no attached terminal) all result in 'no' as
// default
Result_simple prompt_simple(std::string message);

/* Multiline prompt */

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

std::string prompt_multiline(Terminal&,
                             const std::string&,
                             std::vector<std::string>&,
                             std::function<bool(std::string)>&);
}  // namespace Term
