/*
 * This example shows how to interpret keys presses from Terminal.
 */
#include <cpp-terminal/terminal.h>

using Term::Terminal;
using Term::Key;

int main() {
    try {
        Terminal term(true, false);
        int rows, cols;
        term.get_term_size(rows, cols);
        std::cout << "Dimension:" << cols << " " << rows << std::endl;
        std::cout << "Press any key ('q' to quit):" << std::endl;
        bool on = true;
        while (on) {
            int key = term.read_key();
            std::string s;
            if (key >= 'a' && key <= 'z') {
                s = (char)(key+'A'-'a'); // Convert to upper case
                if (key == 'q') on = false;
            } else if (key >= 'A' && key <= 'Z') {
                s = (char)key; // Already in upper case
                s = "Shift+" + s;
            } else if (key >= CTRL_KEY('a') && key <= CTRL_KEY('z')) {
                s = (char)(key+'A'-CTRL_KEY('a')); // Convert to upper case
                s = "Ctrl+" + s;
            } else if (key >= ALT_KEY('a') && key <= ALT_KEY('z')) {
                s = (char)(key+'A'-ALT_KEY('a')); // Convert to upper case
                s = "Alt+" + s;
            } else if (key > 0 && !iscntrl(key) && key < 128) {
                s = std::to_string(key);
            } else {
                switch (key) {
                    case Key::BACKSPACE: s = "BACKSPACE"; break;
                    case Key::ENTER: s = "ENTER"; break;
                    case Key::ALT_ENTER: s = "Alt+ENTER"; break;
                    case Key::TAB: s = "TAB"; break;
                    case Key::ARROW_LEFT: s = "ARROW_LEFT"; break;
                    case Key::ARROW_RIGHT: s = "ARROW_RIGHT"; break;
                    case Key::ARROW_UP: s = "ARROW_UP"; break;
                    case Key::ARROW_DOWN: s = "ARROW_DOWN"; break;
                    case Key::CTRL_UP: s = "CTRL_UP"; break;
                    case Key::CTRL_DOWN: s = "CTRL_DOWN"; break;
                    case Key::CTRL_RIGHT: s = "CTRL_RIGHT"; break;
                    case Key::CTRL_LEFT: s = "CTRL_LEFT"; break;
                    case Key::NUMERIC_5: s = "NUMERIC_5"; break;
                    case Key::DEL: s = "DEL"; break;
                    case Key::HOME: s = "HOME"; break;
                    case Key::INSERT: s = "INSERT"; break;
                    case Key::END: s = "END"; break;
                    case Key::PAGE_UP: s = "PAGE_UP"; break;
                    case Key::PAGE_DOWN: s = "PAGE_DOWN"; break;
                    case Key::ESC: s = "ESC"; break;
                    case Key::F1: s = "F1"; break;
                    case Key::F2: s = "F2"; break;
                    case Key::F3: s = "F3"; break;
                    case Key::F4: s = "F4"; break;
                    case Key::F5: s = "F5"; break;
                    case Key::F6: s = "F6"; break;
                    case Key::F7: s = "F7"; break;
                    case Key::F8: s = "F8"; break;
                    case Key::F9: s = "F9"; break;
                    case Key::F10: s = "F10"; break;
                    case Key::F11: s = "F11"; break;
                    case Key::F12: s = "F12"; break;

                    default: s = "Unknown:" + std::to_string(key);
                }
            }
            std::cout << "Key: " << s << std::endl;
        }
    } catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch(...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
