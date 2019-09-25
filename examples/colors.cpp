#include "terminal.h"

using Term::Terminal;
using Term::color;
using Term::fg;
using Term::bg;
using Term::style;

int main() {
    try {
        Terminal term;
        std::string text = "Some text with "
            + color(fg::red) + color(bg::green) + "red on green"
            + color(bg::reset) + color(fg::reset) + " and some "
            + color(style::bold) + "bold text" + color(style::reset) + ".";
        std::cout << text << std::endl;
    } catch(...) {
        throw;
    }
    return 0;
}
