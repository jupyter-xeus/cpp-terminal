#include <cpp-terminal/terminal.h>
#include <cpp-terminal/version.h>

using Term::Terminal;
using Term::color;
using Term::color24;
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

        std::string rgb_text = "Some Text in "
            + color24(255, 0, 0, true) + 'R'
            + color24(0, 255, 0, true) + 'G'
            + color24(0, 0, 255, true) + 'B'
            + color(fg::reset);

        std::cout << rgb_text << std::endl;

        std::cout << "A color chart: \n";

        for (unsigned int i = 0; i <= 255; i += 3)
        {
            std::cout << color24(i, 0, 0, false)
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3)
        {
            std::cout << color24(0, i, 0, false)
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3)
        {
            std::cout << color24(0, 0, i, false)
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3)
        {
            std::cout << color24(i, i, i, false)
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";

    } catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch(...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
