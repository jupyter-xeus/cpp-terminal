#include "terminal.h"

using Term::Terminal;
using Term::Key;
using Term::move_cursor;

std::string render(int row, int cols, const std::string &prompt_string,
        const std::string &input) {
    int col = prompt_string.size() + input.size() + 1;
    std::string out = move_cursor(row, 1) + prompt_string + input;
    for (size_t i=0; i < cols-out.size(); i++) {
        out.append(" ");
    }
    out.append(move_cursor(row, col));
    return out;
}

std::string prompt(const Terminal &term, const std::string &prompt_string) {
    int row, col;
    term.get_cursor_position(row, col);
    int rows, cols;
    term.get_term_size(rows, cols);
    std::string input, s;
    int key;
    std::cout << render(row, cols, prompt_string, input) << std::flush;
    while ((key = term.read_key()) != Key::ENTER) {
        if (  (key >= 'a' && key <= 'z') ||
              (key >= 'A' && key <= 'Z') ||
              (!iscntrl(key) && key < 128)  ) {
            s = (char)key;
            input.append(s);
        } else if (key == CTRL_KEY('d')) {
            if (input.size() == 0) {
                s = (char)CTRL_KEY('d');
                input.append(s);
                break;
            }
        }
        switch (key) {
            case Key::BACKSPACE: input = input.substr(0, input.size()-1); break;
            case Key::ARROW_LEFT: col--; break;
            case Key::ARROW_RIGHT: col++; break;
        }

        std::cout << render(row, cols, prompt_string, input) << std::flush;
    }
    std::cout << "\n" << std::flush;
    return input;
}

int main() {
    try {
        Terminal term(true, false);
        std::cout << "Interactive prompt." << std::endl;
        std::cout << "  * Use Ctrl-D to exit." << std::endl;
        std::cout << "  * Use Enter to submit." << std::endl;
        std::cout << "  * Features:" << std::endl;
        std::cout << "    - Multi-line editing (use Alt-Enter to add a new line)" << std::endl;
        std::cout << "    - History (Up and Down arrows)" << std::endl;
        std::cout << "    - Syntax highlighting" << std::endl;
        while (true) {
            std::string answer = prompt(term, "> ");
            if (answer.size() == 1 && answer[0] == CTRL_KEY('d')) break;
            std::cout << "Submitted text: " << answer << std::endl;
        }
    } catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch(...) {
        std::cerr << "Unkown error." << std::endl;
        return 1;
    }
    return 0;
}
