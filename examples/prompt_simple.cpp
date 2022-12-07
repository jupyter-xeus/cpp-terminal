#include <cpp-terminal/prompt.hpp>
#include <cpp-terminal/version.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
int main() {
    std::cout << "Running cpp-terminal version: "
              << CPP_TERMINAL_VERSION_COMPLETE << std::endl;
    try {
        std::cout << "CPP-Terminal basic prompt example: \n\n";
        switch (Term::prompt_simple("Proceed?")) {
            case Term::Result_simple::YES:
                std::cout << "Proceeding...\n";
                break;
            case Term::Result_simple::NO:
                std::cout << "Stopping...\n";
                break;
            case Term::Result_simple::ABORT:
                std::cout << "Exit signal received, exiting now...\n";
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
