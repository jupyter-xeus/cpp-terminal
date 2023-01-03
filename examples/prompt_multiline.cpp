#include <cpp-terminal/input.hpp>
#include <cpp-terminal/prompt.hpp>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using Term::Key;
using Term::prompt_multiline;
using Term::Terminal;

bool determine_completeness(CPP_TERMINAL_MAYBE_UNUSED const std::string& command)
{
  // Determine if the statement is complete
  if(command.size() > 1 && command.substr(command.size() - 2, 1) == "\\") return false;
  else
    return true;
}

int main()
{
  try
  {
    if(!Term::stdin_connected())
    {
      std::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Terminal term(false, true, false, false);
    std::cout << "Interactive prompt.\n"
              << "  * Use Ctrl-D to exit.\n"
              << "  * Use Enter to submit.\n"
              << "  * Features:\n"
              << "    - Editing (Keys: Left, Right, Home, End, Backspace)\n"
              << "    - History (Keys: Up, Down)\n"
              << "    - Multi-line editing (use Alt-Enter to add a new line)" << std::endl;
    // TODO:
    // std::cout << "    - Syntax highlighting" << std::endl;
    std::vector<std::string>         history;
    std::function<bool(std::string)> iscomplete = determine_completeness;
    while(true)
    {
      std::string answer = Term::prompt_multiline("> ", history, iscomplete);
      if(answer.size() == 1 && answer[0] == Key::CTRL_D) break;
      std::cout << "Submitted text: " << answer << std::endl;
    }
  }
  catch(const std::runtime_error& re)
  {
    std::cerr << "Runtime error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
