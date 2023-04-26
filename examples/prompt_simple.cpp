#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/prompt.hpp"
#include "cpp-terminal/version.hpp"
#include "cpp-terminal/terminal.hpp"

int main()
{
  Term::terminal << "Running cpp-terminal version: " << Term::Version << std::endl;
  try
  {
    Term::terminal << "CPP-Terminal basic prompt example: \n\n";
    switch(Term::prompt_simple("Proceed?"))
    {
      case Term::Result_simple::YES: Term::terminal << "Proceeding...\n"; break;
      case Term::Result_simple::NO: Term::terminal << "Stopping...\n"; break;
      case Term::Result_simple::ABORT: Term::terminal << "Exit signal received, exiting now...\n"; break;
    }
  }
  catch(const Term::Exception& re)
  {
    Term::terminal << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    Term::terminal << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
