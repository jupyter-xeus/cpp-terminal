#include <cpp-terminal/base.hpp>
#include <cpp-terminal/input.hpp>
#include <iostream>

void render(size_t rows, size_t cols, uint8_t cursor_pos) {
    // calculate the menu location
    size_t rows_start = rows / 2 - 5;
    size_t cols_start = cols / 2 - 6;

    // set up RGB colors, we'll use different ways to create colors, you might
    // want to pick one method of your choice
    // Use pre-defined 4bit color
    Term::RGB border_color = Term::bit4_to_rgb(Term::fg::red);
    // use 24bit colors directly using the function
    Term::RGB item_bg = Term::bit24_to_rgb(255, 153, 51);
    // set 24bit color directly when creating the struct
    Term::RGB item_fg{255, 255, 255};

    // print the menu
    std::cout << Term::move_cursor(rows_start, cols_start)
              << Term::color24_auto_fg(border_color) << "┌───────────┐"
              << Term::color(Term::fg::reset);
    rows_start++;
    for (uint8_t i = 0; i < 9; i++) {
        std::cout << Term::move_cursor(rows_start, cols_start)
                  << Term::color24_auto_fg(border_color) << "│";

        if (i + 1 == cursor_pos) {
            // you can also set color directly in the color function
            std::cout << Term::color24_auto_bg({0, 255, 0})
                      << Term::color24_auto_fg(item_fg) << "  " << i + 1
                      << ": Item  " << Term::color(Term::bg::reset);
        } else {
            std::cout << Term::color24_auto_bg(item_bg)
                      << Term::color24_auto_fg(item_fg) << "  " << i + 1
                      << ": Item  " << Term::color(Term::bg::reset);
        }

        std::cout << Term::color24_auto_fg(border_color) << "│"
                  << Term::color(Term::style::reset);
        rows_start++;
    }
    std::cout << Term::move_cursor(rows_start, cols_start)
              << Term::color24_auto_fg(border_color) << "└───────────┘"
              << Term::color(Term::fg::reset);

    // flush the output stream, changes won't appear when this isn't done
    std::cout << std::flush;
}

int main() {
    try {
        // preparing the terminal
        Term::Terminal term(true, true, true, true);

        int rows, cols;
        Term::get_term_size(rows, cols);
        int cursor_pos = 1;

        render(rows, cols, cursor_pos);

        int key;
        bool running = true;
        while (running) {
            render(rows, cols, cursor_pos);

            key = Term::read_key();
            switch (key) {
                case Term::Key::ARROW_UP:
                    if (cursor_pos > 1) {
                        cursor_pos--;
                    }
                    break;
                case Term::Key::ARROW_DOWN:
                    if (cursor_pos < 9) {
                        cursor_pos++;
                    }
                    break;
                case Term::Key::ESC:
                case 'q':
                case Term::Key::CTRL + 'c':
                    running = false;
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
