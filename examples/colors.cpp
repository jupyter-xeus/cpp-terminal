#include <cpp-terminal/terminal.h>
#include <cpp-terminal/version.h>

using Term::Terminal;
using Term::color;
using Term::fg;
using Term::bg;
using Term::style;

int main() {
    std::cout << "Running cpp-terminal version: " 
                << CPP_TERMINAL_VERSION_COMPLETE
                << std::endl;
    try {
        Terminal term;
        if (Terminal::is_stdout_a_tty()) {
            std::cout << "Standard output is attached to a terminal."
                << std::endl;
        } else {
            std::cout << "Standard output is not attached to a terminal."
                << std::endl;
        }
        std::string text = "Some text with "
            + color(fg::red) + color(bg::green) + "red on green"
            + color(bg::reset) + color(fg::reset) + " and some "
            + color(style::bold) + "bold text" + color(style::reset) + ".\n";
        text += "Unicode works too: originally written by Ondřej Čertík.";
        std::cout << text << std::endl;
    } catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch(...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
