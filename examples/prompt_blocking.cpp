#include <cpp-terminal/version.h>
#include <cpp-terminal/prompt.hpp>
#include <iostream>

int main() {
    std::cout << "Running cpp-terminal version: "
              << CPP_TERMINAL_VERSION_COMPLETE << std::endl;
    try {
        std::cout << "CPP-Terminal basic prompt example: \n\n";
        switch (Term::prompt_blocking("Proceed?", "Y", "n", ":")) {
            case Term::Result::NONE:  // no input was given
                std::cout << "No input given, proceeding anyway...\n";
                break;
            case Term::Result::INVALID:
                std::cout << "Invalid input given, proceeding anyway\n";
                break;
            case Term::Result::YES:
                std::cout << "Proceeding...\n";
                break;
            case Term::Result::NO:
                std::cout << "Stopping...\n";
                break;
            case Term::Result::ABORT:
                std::cout << "Exit signal received, exiting now...\n";
                break;
            case Term::Result::ERROR:
                std::cout << "Error while capturing input, is your terminal "
                             "attached to a TTY?\n";
                std::cout << "Aborting...\n";
                break;
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
