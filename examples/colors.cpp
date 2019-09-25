#include "terminal.h"

using Term::Terminal;
using Term::cursor_on;
using Term::cursor_off;
using Term::move_cursor;
using Term::color;
using Term::fg;
using Term::bg;
using Term::style;
using Term::Key;

void render(const Terminal &term, int rows, int cols, int pos)
{
    std::string scr;
    scr.reserve(16*1024);

    scr.append(cursor_off());
    scr.append(move_cursor(1, 1));

    for (int i=1; i <= rows; i++) {
        if (i == pos) {
            scr.append(color(fg::red));
            scr.append(color(bg::gray));
            scr.append(color(style::bold));
        } else {
            scr.append(color(fg::blue));
            scr.append(color(bg::green));
        }
        scr.append(std::to_string(i) + ": item");
        scr.append(color(bg::reset));
        scr.append(color(fg::reset));
        scr.append(color(style::reset));
        if (i < rows) scr.append("\n");
    }

    scr.append(move_cursor(rows / 2, cols / 2));

    scr.append(cursor_on());

    term.write(scr);
}

int main() {
    try {
        Terminal term(false);
        std::string text = "Some text with "
            + color(fg::red) + color(bg::green) + "red on green"
            + color(bg::reset) + color(fg::reset) + " and some "
            + color(style::bold) + "bold text" + color(style::reset) + ".";
        std::cout << text << std::endl;
    } catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
    } catch(...) {
        std::cerr << "Unkown error: " << std::endl;
    }
    return 0;
}
