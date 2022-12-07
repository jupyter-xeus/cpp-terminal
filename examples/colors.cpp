#include <iostream>
#include <stdexcept>
#include "cpp-terminal/base.hpp"
#include "cpp-terminal/version.hpp"

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
            color_bg(Term::Color4::DEFAULT) + color_fg(Term::Color4::DEFAULT) +
            " and some " + style(Term::Style::BOLD) + "bold text" +
            style(Term::Style::RESET) + ".\n";
        text += "Unicode works too: originally written by Ondřej Čertík.";
        std::cout << text << std::endl;

        std::string rgb_text = "Some Text in " + Term::color_fg(255, 0, 0) +
                               'R' + Term::color_fg(0, 255, 0) + 'G' +
                               Term::color_fg(0, 0, 255) + 'B' +
                               Term::color_fg(Term::Color4::DEFAULT);

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

        std::cout << "\nColor conversion (4bit base):\n";
        std::cout << "4bit original: " << Term::color_bg(Term::Color4::BLACK)
                  << " " << Term::color_bg(Term::Color4::RED) << " "
                  << Term::color_bg(Term::Color4::GREEN) << " "
                  << Term::color_bg(Term::Color4::YELLOW) << " "
                  << Term::color_bg(Term::Color4::BLUE) << " "
                  << Term::color_bg(Term::Color4::MAGENTA) << " "
                  << Term::color_bg(Term::Color4::CYAN) << " "
                  << Term::color_bg(Term::Color4::WHITE) << " "
                  << Term::color_bg(Term::Color4::GRAY) << " "
                  << Term::color_bg(Term::Color4::RED_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::GREEN_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::YELLOW_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::BLUE_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::MAGENTA_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::CYAN_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::WHITE_BRIGHT) << " "
                  << Term::color_bg(Term::Color4::DEFAULT) << "\n";
        std::cout
            << "4bit to 4bit:  "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::BLACK)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::RED)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::GREEN)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::YELLOW)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::BLUE)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::MAGENTA)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::CYAN)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::WHITE)))
            << " "
            << Term::color_bg(
                   Term::rgb_to_bit4(Term::bit4_to_rgb(Term::Color4::GRAY)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::RED_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::GREEN_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::YELLOW_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::BLUE_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::MAGENTA_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::CYAN_BRIGHT)))
            << " "
            << Term::color_bg(Term::rgb_to_bit4(
                   Term::bit4_to_rgb(Term::Color4::WHITE_BRIGHT)))
            << " " << Term::color_bg(Term::Color4::DEFAULT) << "\n";

        std::cout
            << "4bit to 24bit: "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::BLACK)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::RED)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::GREEN)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::YELLOW)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::BLUE)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::MAGENTA)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::CYAN)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::WHITE)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::GRAY)) << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::RED_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::GREEN_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::YELLOW_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::BLUE_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::MAGENTA_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::CYAN_BRIGHT))
            << " "
            << Term::color_bg(Term::bit4_to_rgb(Term::Color4::WHITE_BRIGHT))
            << " " << Term::color_bg(Term::Color4::DEFAULT) << "\n";

        std::cout
            << "4bit to 8bit:  "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::BLACK))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::RED))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::GREEN))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::YELLOW))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::BLUE))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::MAGENTA))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::CYAN))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::WHITE))) +
                   " "
            << Term::color_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(Term::Color4::GRAY))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::RED_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::GREEN_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::YELLOW_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::BLUE_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::MAGENTA_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::CYAN_BRIGHT))) +
                   " "
            << Term::color_bg(Term::rgb_to_bit8(
                   Term::bit4_to_rgb(Term::Color4::WHITE_BRIGHT))) +
                   " "
            << Term::color_bg(Term::Color4::DEFAULT) << "\n";

        std::cout << "\nColor conversion (24bit)\n";
        /* red color space */
        std::cout << "24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(i, 0, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(i, 0, 0)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit4(Term::bit24_to_rgb(i, 0, 0)))
                      << " "
                      << "\033[0m";
        }
        /* green color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(0, i, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(0, i, 0)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit4(Term::bit24_to_rgb(0, i, 0)))
                      << " "
                      << "\033[0m";
        }
        /* blue color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(0, 0, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(0, 0, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit4(Term::bit24_to_rgb(0, 0, i)))
                      << " "
                      << "\033[0m";
        }
        /* black / grey color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(i, i, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(i, i, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_bg(
                             Term::rgb_to_bit4(Term::bit24_to_rgb(i, i, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";

        std::cout << "\nAuto color for 24bit: \n";

        // bit24_support doesn't work in CI
        if (Term::stdout_connected()) {
            if (Term::bit24_support()) {
                std::cout << "24bit support: yes\n$COLORTERM: "
                          << Term::Private::getenv("COLORTERM") << '\n';
            } else {
                std::cout << "24bit support: no\n$COLORTERM: "
                          << Term::Private::getenv("COLORTERM") << '\n';
            }
        } else {
            std::cout << "24bit support: Not connected to a terminal\n";
        }
        std::cout << "24bit auto function test:\n";

        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_auto(Term::rgbf_bg(i, 0, 0)) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_auto(Term::rgbf_bg(0, i, 0)) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_auto(Term::rgbf_bg(0, 0, i)) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color_auto(Term::rgbf_bg(i, i, i)) << " "
                      << "\033[0m";
        }
        std::cout << "\n";

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
