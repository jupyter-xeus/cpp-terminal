#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/version.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  Term::terminal << "Running cpp-terminal version: " << Term::Version << std::endl;
  try
  {
    Term::terminal << "CPP-Terminal basic prompt example: \n\n";
    switch(Term::prompt("Proceed?", "Y", "n", ":", true))
    {
      case Term::Result::NONE:  // no input was given
        Term::terminal << "No input given, proceeding anyway...\n";
        break;
      case Term::Result::INVALID: Term::terminal << "Invalid input given, proceeding anyway\n"; break;
      case Term::Result::YES: Term::terminal << "Proceeding...\n"; break;
      case Term::Result::NO: Term::terminal << "Stopping...\n"; break;
      case Term::Result::ABORT: Term::terminal << "Exit signal received, exiting now...\n"; break;
      case Term::Result::ERROR:
        Term::terminal << "Error while capturing input, is your terminal attached to a TTY?\n";
        Term::terminal << "Aborting...\n";
        break;
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
