#include "terminal.h"

using Term::Terminal;

std::string prompt(const std::string &s) {
    std::cout << s;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int main() {
    try {
        Terminal term;
        while (true) {
            std::string answer = prompt("Give me some input (ENTER to exit): ");
            std::cout << "You said: " << answer << std::endl;
            if (answer == "") break;
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
