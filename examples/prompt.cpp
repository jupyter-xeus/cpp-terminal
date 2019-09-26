#include "terminal.h"

using Term::Terminal;
using Term::Key;
using Term::move_cursor;

// This model contains all the information about the state of the prompt in an
// abstract way, irrespective of where or how it is rendered.
struct Model {
    std::string prompt_string; // The string to show as the prompt
    std::string input; // The current input string in the prompt
    // The current cursor position in the "input" string, starting from (1,1)
    size_t cursor_col, cursor_row;
};

std::string render(const Model &m, int prompt_row, int term_cols) {
    std::string out = move_cursor(prompt_row, 1) + m.prompt_string + m.input;
    for (size_t i=0; i < term_cols-out.size(); i++) {
        out.append(" ");
    }
    out.append(move_cursor(prompt_row+m.cursor_row-1,
        m.prompt_string.size() + m.cursor_col));
    return out;
}

std::string prompt(const Terminal &term, const std::string &prompt_string) {
    int row, col;
    term.get_cursor_position(row, col);
    int rows, cols;
    term.get_term_size(rows, cols);

    Model m;
    m.prompt_string = prompt_string;
    m.cursor_col = 1;
    m.cursor_row = 1;

    int key;
    std::cout << render(m, row, cols) << std::flush;
    while ((key = term.read_key()) != Key::ENTER) {
        if (  (key >= 'a' && key <= 'z') ||
              (key >= 'A' && key <= 'Z') ||
              (!iscntrl(key) && key < 128)  ) {
            std::string before = m.input.substr(0, m.cursor_col-1);
            std::string newchar; newchar.push_back(key);
            std::string after = m.input.substr(m.cursor_col-1);
            m.input = before + newchar + after;
            m.cursor_col++;
        } else if (key == CTRL_KEY('d')) {
            if (m.input.size() == 0) {
                m.input.push_back(CTRL_KEY('d'));
                break;
            }
        } else {
            switch (key) {
                case Key::BACKSPACE:
                    if (m.cursor_col > 1) {
                        std::string before = m.input.substr(0, m.cursor_col-2);
                        std::string after = m.input.substr(m.cursor_col-1);
                        m.input = before + after;
                        m.cursor_col--;
                    }
                    break;
                case Key::ARROW_LEFT:
                    if (m.cursor_col > 1) {
                        m.cursor_col--;
                    }
                    break;
                case Key::ARROW_RIGHT:
                    if (m.cursor_col <= m.input.size()) {
                        m.cursor_col++;
                    }
                    break;
                case Key::HOME:
                    m.cursor_col = 1;
                    break;
                case Key::END:
                    m.cursor_col = m.input.size()+1;
                    break;
            }
        }
        std::cout << render(m, row, cols) << std::flush;
    }
    std::cout << "\n" << std::flush;
    return m.input;
}

int main() {
    try {
        Terminal term(true, false);
        std::cout << "Interactive prompt." << std::endl;
        std::cout << "  * Use Ctrl-D to exit." << std::endl;
        std::cout << "  * Use Enter to submit." << std::endl;
        std::cout << "  * Features:" << std::endl;
        std::cout << "    - Keys: Left, Right, Home, End, Backspace" << std::endl;
        // TODO:
        //std::cout << "    - Multi-line editing (use Alt-Enter to add a new line)" << std::endl;
        //std::cout << "    - History (Up and Down arrows)" << std::endl;
        //std::cout << "    - Syntax highlighting" << std::endl;
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
