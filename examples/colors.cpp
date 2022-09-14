#include <cpp-terminal/version.h>
#include <cpp-terminal/base.hpp>
#include <iostream>

int main() {
    std::cout << "Running cpp-terminal version: "
              << CPP_TERMINAL_VERSION_COMPLETE << std::endl;
    try {
        Term::Terminal term(false);
        if (Term::stdout_connected()) {
            std::cout << "Standard output is attached to a terminal."
                      << std::endl;
        } else {
            std::cout << "Standard output is not attached to a terminal."
                      << std::endl;
        }
        std::string text =
            "Some text with " + Term::color_fg(Term::Color4::RED) +
            color_bg(Term::Color4::GREEN) + "red on green" +
            color_bg(Term::Color4::NONE) + color_fg(Term::Color4::NONE) +
            " and some " + style(Term::Style::BOLD) + "bold text" +
            style(Term::Style::RESET) + ".\n";
        text += "Unicode works too: originally written by Ondřej Čertík.";
        std::cout << text << std::endl;

        std::string rgb_text = "Some Text in " + Term::color_fg(255, 0, 0) +
                               'R' + Term::color_fg(0, 255, 0) + 'G' +
                               Term::color_fg(0, 0, 255) + 'B' +
                               Term::color_fg(Term::Color4::NONE);

        std::cout << rgb_text << std::endl;

        std::cout << "A color chart: \n";

        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(i, 0, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(0, i, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(0, 0, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(i, i, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        std::cout << "Press any key to quit" << std::endl;
        if (Term::stdout_connected())
            std::cin.get();

    } catch (const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
