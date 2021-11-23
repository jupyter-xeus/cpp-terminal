#include <cpp-terminal/window.hpp>
#include <iostream>
#include <cpp-terminal/base.hpp>
#include <cpp-terminal/input.hpp>

using Term::bg;
using Term::fg;
using Term::Key;
using Term::style;
using Term::Terminal;

std::string render(Term::Window_24bit& scr,
                   int rows,
                   int cols,
                   int menuheight,
                   int menuwidth,
                   int menupos) {
    scr.clear();
    int menux0 = (cols - menuwidth) / 2 + 1;
    int menuy0 = (rows - menuheight) / 2 + 1;
    scr.print_rect(menux0, menuy0, menux0 + menuwidth + 1,
                   menuy0 + menuheight + 1);

    for (int i = 1; i <= menuheight; i++) {
        std::string s = std::to_string(i) + ": item";
        scr.print_str(menux0 + 1, menuy0 + i, s);
        if (i == menupos) {
            scr.fill_fg(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i,
                        184, 37, 0);
            scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i,
                        8, 9, 10);
            scr.fill_style(menux0 + 1, menuy0 + i, menux0 + s.size(),
                           menuy0 + i, style::bold);
        } else {
            scr.fill_fg(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i,
                        5, 63, 97);
            scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i,
                        142, 166, 4);
        }
    }

    int y = menuy0 + menuheight + 5;
    scr.print_str(1, y, "Selected item: " + std::to_string(menupos));
    scr.print_str(1, y + 1, "Menu width: " + std::to_string(menuwidth));
    scr.print_str(1, y + 2, "Menu height: " + std::to_string(menuheight));
    scr.print_str(1, y + 3, "Unicode test: Ondřej Čertík, ἐξήκοι");

    return scr.render(1, 1, true);
}

int main() {
    try {
        Terminal term(true, true, false);
        int rows{}, cols{};
        Term::get_term_size(rows, cols);
        int pos = 5;
        int h = 10;
        int w = 10;
        bool on = true;
        Term::Window_24bit scr(cols, rows);
        while (on) {
            std::cout << render(scr, rows, cols, h, w, pos) << std::flush;
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
