#include <cpp-terminal/terminal.h>

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <cpp-terminal/prompt.hpp>

using Term::Key;
using Term::prompt;
using Term::Terminal;

bool determine_completeness([[maybe_unused]] std::string command) {
    // Determine if the statement is complete
    bool complete;
    if (command.substr(command.size() - 2, 1) == "\\") {
        complete = false;
    } else {
        complete = true;
    }
    return complete;
}

int main() {
    try {
        Terminal term(true, false);
        std::cout << "Interactive prompt." << std::endl;
        std::cout << "  * Use Ctrl-D to exit." << std::endl;
        std::cout << "  * Use Enter to submit." << std::endl;
        std::cout << "  * Features:" << std::endl;
        std::cout << "    - Editing (Keys: Left, Right, Home, End, Backspace)"
                  << std::endl;
        std::cout << "    - History (Keys: Up, Down)" << std::endl;
        std::cout
            << "    - Multi-line editing (use Alt-Enter to add a new line)"
            << std::endl;
        // TODO:
        // std::cout << "    - Syntax highlighting" << std::endl;
        std::vector<std::string> history;
        std::function<bool(std::string)> iscomplete = determine_completeness;
        while (true) {
            std::string answer = prompt(term, "> ", history, iscomplete);
            if (answer.size() == 1 && answer[0] == Key::CTRL + 'd')
                break;
            std::cout << "Submitted text: " << answer << std::endl;
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
