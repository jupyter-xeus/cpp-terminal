#include <cpp-terminal/base.hpp>
#include <cpp-terminal/input.hpp>
#include <cpp-terminal/window.hpp>
#include <iostream>
#include <stdexcept>
#include <tuple>

std::string render(Term::Window& scr,
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
                        Term::Color4::RED);  // FG
            scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i,
                        Term::Color4::GRAY);  // BG
            scr.fill_style(menux0 + 1, menuy0 + i, menux0 + s.size(),
                           menuy0 + i, Term::Style::BOLD);
        } else {
            scr.fill_fg(menux0 + 1, menuy0 + i, menux0 + s.size(), menuy0 + i,
                        Term::Color4::BLUE);
            scr.fill_bg(menux0 + 1, menuy0 + i, menux0 + menuwidth, menuy0 + i,
                        Term::Color4::GREEN);
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
        // check if the terminal is capable of handling input
        if (!Term::stdin_connected()) {
            std::cout << "The terminal is not attached to a TTY and therefore "
                         "can't catch user input. Exiting...\n";
            return 1;
        }
        Term::Terminal term(true, true, true, true);
        std::tuple<std::size_t, std::size_t> term_size = Term::get_size();
        int pos = 5;
        int h = 10;
        std::size_t w{10};
        bool on = true;
        Term::Window scr(std::get<1>(term_size), std::get<0>(term_size));
        while (on) {
            std::cout << render(scr, std::get<0>(term_size),
                                std::get<1>(term_size), h, w, pos)
                      << std::flush;
            int key = Term::read_key();
            switch (key) {
                case Term::Key::ARROW_LEFT:
                    if (w > 10)
                        w--;
                    break;
                case Term::Key::ARROW_RIGHT:
                    if (w <
                        static_cast<std::size_t>(std::get<1>(term_size) - 5))
                        w++;
                    break;
                case Term::Key::ARROW_UP:
                    if (pos > 1)
                        pos--;
                    break;
                case Term::Key::ARROW_DOWN:
                    if (pos < h)
                        pos++;
                    break;
                case Term::Key::HOME:
                    pos = 1;
                    break;
                case Term::Key::END:
                    pos = h;
                    break;
                case 'q':
                case Term::Key::ESC:
                case Term::Key::CTRL + 'c':
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
