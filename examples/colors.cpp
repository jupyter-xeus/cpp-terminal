#include <cpp-terminal/version.h>
#include <cpp-terminal/base.hpp>
#include <iostream>

using Term::bg;
using Term::color;
using Term::color24_bg;
using Term::color24_fg;
using Term::fg;
using Term::style;
using Term::Terminal;

int main() {
    std::cout << "Running cpp-terminal version: "
              << CPP_TERMINAL_VERSION_COMPLETE << std::endl;
    try {
        Terminal term(false);
        if (Term::is_stdout_a_tty()) {
            std::cout << "Standard output is attached to a terminal."
                      << std::endl;
        } else {
            std::cout << "Standard output is not attached to a terminal."
                      << std::endl;
        }
        std::string text = "Some text with " + color(fg::red) +
                           color(bg::green) + "red on green" +
                           color(bg::reset) + color(fg::reset) + " and some " +
                           color(style::bold) + "bold text" +
                           color(style::reset) + ".\n";
        text += "Unicode works too: originally written by Ondřej Čertík.";
        std::cout << text << std::endl;

        std::string rgb_text = "Some Text in " + color24_fg(255, 0, 0) + 'R' +
                               color24_fg(0, 255, 0) + 'G' +
                               color24_fg(0, 0, 255) + 'B' + color(fg::reset);

        std::cout << rgb_text << std::endl;

        std::cout << "A color chart: \n";

        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(i, 0, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(0, i, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(0, 0, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(i, i, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";

        std::cout << "\nColor conversion (4bit base):\n";
        std::cout << "4bit original: " << color(bg::black) << " "
                  << color(bg::red) << " " << color(bg::green) << " "
                  << color(bg::yellow) << " " << color(bg::blue) << " "
                  << color(bg::magenta) << " " << color(bg::cyan) << " "
                  << color(bg::white) << " " << color(bg::gray) << " "
                  << color(bg::bright_red) << " " << color(bg::bright_green)
                  << " " << color(bg::bright_yellow) << " "
                  << color(bg::bright_blue) << " " << color(bg::bright_magenta)
                  << " " << color(bg::bright_cyan) << " "
                  << color(bg::bright_white) << " " << color(bg::reset) << "\n";
        std::cout
            << "4bit to 4bit:  "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::black))) << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::red))) << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::green))) << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::yellow))) << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::blue))) << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::magenta)))
            << " " << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::cyan)))
            << " " << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::white)))
            << " " << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::gray)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_red)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_green)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_yellow)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_blue)))
            << " "
            << color(
                   Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_magenta)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_cyan)))
            << " "
            << color(Term::rgb_to_bit4_bg(Term::bit4_to_rgb(bg::bright_white)))
            << " " << color(bg::reset) << "\n";

        std::cout << "4bit to 24bit: "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::black)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::red)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::green)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::yellow)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::blue)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::magenta)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::cyan)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::white)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::gray)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::bright_red)) << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::bright_green))
                  << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::bright_yellow))
                  << " " << Term::color24_bg(Term::bit4_to_rgb(bg::bright_blue))
                  << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::bright_magenta))
                  << " " << Term::color24_bg(Term::bit4_to_rgb(bg::bright_cyan))
                  << " "
                  << Term::color24_bg(Term::bit4_to_rgb(bg::bright_white))
                  << " " << Term::color(bg::reset) << "\n";

        std::cout
            << "4bit to 8bit:  "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::black))) +
                   " "
            << Term::color8_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(bg::red))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::green))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::yellow))) +
                   " "
            << Term::color8_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(bg::blue))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::magenta))) +
                   " "
            << Term::color8_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(bg::cyan))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::white))) +
                   " "
            << Term::color8_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(bg::gray))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_red))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_green))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_yellow))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_blue))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_magenta))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_cyan))) +
                   " "
            << Term::color8_bg(
                   Term::rgb_to_bit8(Term::bit4_to_rgb(bg::bright_white))) +
                   " "
            << Term::color(bg::reset) << "\n";

        std::cout << "\nColor conversion (24bit)\n";
        /* red color space */
        std::cout << "24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(i, 0, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color8_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(i, 0, 0)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color(
                             Term::rgb_to_bit4_bg(Term::bit24_to_rgb(i, 0, 0)))
                      << " "
                      << "\033[0m";
        }
        /* green color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(0, i, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color8_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(0, i, 0)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color(
                             Term::rgb_to_bit4_bg(Term::bit24_to_rgb(0, i, 0)))
                      << " "
                      << "\033[0m";
        }
        /* blue color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(0, 0, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color8_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(0, 0, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color(
                             Term::rgb_to_bit4_bg(Term::bit24_to_rgb(0, 0, i)))
                      << " "
                      << "\033[0m";
        }
        /* black / grey color space */
        std::cout << "\n24bit original: ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << color24_bg(i, i, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 8bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color8_bg(
                             Term::rgb_to_bit8(Term::bit24_to_rgb(i, i, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n24bit to 4bit:  ";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color(
                             Term::rgb_to_bit4_bg(Term::bit24_to_rgb(i, i, i)))
                      << " "
                      << "\033[0m";
        }
        std::cout << "\n";

        std::cout << "\nAuto color for 24bit: \n";
        if (Term::bit24_support()) {
            std::cout << "24bit support: yes\n$COLORTERM: "
                      << getenv("COLORTERM") << '\n';
        } else {
            std::cout << "24bit support: no\n$COLORTERM: "
                      << getenv("COLORTERM") << '\n';
        }
        std::cout << "24bit auto function test:\n";

        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color24_auto_bg(i, 0, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color24_auto_bg(0, i, 0) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color24_auto_bg(0, 0, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";
        for (unsigned int i = 0; i <= 255; i += 3) {
            std::cout << Term::color24_auto_bg(i, i, i) << " "
                      << "\033[0m";
        }
        std::cout << "\n";

    } catch (const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    return 0;
}
