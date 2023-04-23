#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/tty.hpp"

#include <iostream>
#include <string>
#include <vector>

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
    if(!Term::is_stdin_a_tty())
    {
      std::cout << "The terminal is not attached to a TTY and therefore can't catch user input. Exiting...\n";
      return 1;
    }
    Term::terminal.setOptions({Term::Option::NoClearScreen, Term::Option::SignalKeys, Term::Option::Cursor, Term::Option::Raw});
    std::cout << "Interactive prompt.\n"
              << "  * Use Ctrl-D to exit.\n"
              << "  * Use Enter to submit.\n"
              << "  * Features:\n"
              << "    - Editing (Keys: Left, Right, Home, End, Backspace)\n"
              << "    - History (Keys: Up, Down)\n"
              << "    - Multi-line editing (use CTRL+N to add a new line)" << std::endl;
    // TODO:
    // std::cout << "    - Syntax highlighting" << std::endl;
    std::vector<std::string>         history;
    std::function<bool(std::string)> iscomplete = determine_completeness;
    while(true)
    {
      std::string answer = Term::prompt_multiline("> ", history, iscomplete);
      if(answer.size() == 1 && answer[0] == Term::Key::CTRL_D) break;
      std::cout << "Submitted text: " << answer << std::endl;
    }
  }
  catch(const Term::Exception& re)
  {
    std::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
