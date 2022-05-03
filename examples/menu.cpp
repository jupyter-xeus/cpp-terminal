#include <cpp-terminal/base.hpp>
#include <cpp-terminal/input.hpp>
#include <iostream>

using Term::bg;
using Term::color;
using Term::cursor_off;
using Term::cursor_on;
using Term::fg;
using Term::Key;
using Term::move_cursor;
using Term::style;
using Term::Terminal;

void render(int rows, int cols, int menuheight, int menuwidth, int menupos) {
    std::string scr;
    scr.reserve(16 * 1024);

    scr.append(move_cursor(1, 1));

    int menux0 = (cols - menuwidth) / 2;
    int menuy0 = (rows - menuheight) / 2;

    for (int j = 1; j <= menuy0; j++) {
        scr.append("\n");
    }

    for (int j = 1; j <= menux0; j++) {
        scr.append(" ");
    }
    scr.append("┌");
    for (int j = 1; j <= menuwidth; j++) {
        scr.append("─");
    }
    scr.append("┐");
    scr.append(" \n");
    for (int i = 1; i <= menuheight; i++) {
        for (int j = 1; j <= menux0; j++) {
            scr.append(" ");
        }
        scr.append("│");
        if (i == menupos) {
            scr.append(color(fg::red));
            scr.append(color(bg::gray));
            scr.append(color(style::bold));
        } else {
            scr.append(color(fg::blue));
            scr.append(color(bg::green));
        }
        std::string s = std::to_string(i) + ": item";
        scr.append(s);
        for (size_t j = 1; j <= menuwidth - s.size(); j++) {
            scr.append(" ");
        }
        scr.append(color(bg::reset));
        scr.append(color(fg::reset));
        scr.append(color(style::reset));
        scr.append("│");
        scr.append(" \n");
    }
    for (int j = 1; j <= menux0; j++) {
        scr.append(" ");
    }
    scr.append("└");
    for (int j = 1; j <= menuwidth; j++) {
        scr.append("─");
    }
    scr.append("┘");
    scr.append(" \n");

    scr.append(move_cursor(menuy0 + menuheight + 5, 1));
    scr.append("Selected item: " + std::to_string(menupos) + "      \n");
    scr.append("Menu width: " + std::to_string(menuwidth) + "       \n");
    scr.append("Menu height: " + std::to_string(menuheight) + "    \n");

    std::cout << scr << std::flush;
}

int main() {
    try {
        Terminal term(true, true, false, true);
        int rows{}, cols{};
        Term::get_term_size(rows, cols);
        int pos = 5;
        int h = 10;
        int w = 10;
        bool on = true;
        while (on) {
            render(rows, cols, h, w, pos);
            int key = Term::read_key();
            switch (key) {
                case Key::ARROW_LEFT:
                    if (w > 10)
                        w--;
                    break;
                case Key::ARROW_RIGHT:
                    if (w < cols - 5)
                        w++;
                    break;
                case Key::ARROW_UP:
                    if (pos > 1)
                        pos--;
                    break;
                case Key::ARROW_DOWN:
                    if (pos < h)
                        pos++;
                    break;
                case Key::HOME:
                    pos = 1;
                    break;
                case Key::END:
                    pos = h;
                    break;
                case 'q':
                case Key::ESC:
                    on = false;
                    break;
            }
        }
    } catch (const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
