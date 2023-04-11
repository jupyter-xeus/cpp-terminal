#include "cpp-terminal/input.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  Term::terminal.setOptions({Term::Option::Raw});
  Term::terminal << "This should be printed" << std::flush << " only in terminal" << std::endl;
  std::cout << "This should be printed in terminal if stdout is not redirected" << std::endl;
  Term::terminal << "Now type a number and then a string !" << std::endl;
  int         number;
  std::string string;
  Term::terminal >> number >> string;
  std::cout << "Your number is " << number << " " << string << std::endl;
  std::cout << "Now we are back to raw mode. Play with keys and press CTRL_C to quit." << std::endl;
  Term::Key event;
  while((event = Term::read_event()) != Term::Key(Term::Key::CTRL_C))
  {
    if(event.is_extended_ASCII()) Term::terminal << "You typed :" << static_cast<char>(event) << std::endl;
  }
  return 0;
}
