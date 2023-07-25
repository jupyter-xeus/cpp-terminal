#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>

int main()
{
  Term::terminal.setOptions(Term::Option::Raw);
  std::cout << "Now type a number and then a string !" << std::endl;
  int         number{0};
  std::string string;
  Term::cin >> number;
  Term::cin >> string;
  std::cout << "Your number is " << number << " and string is " << string << std::endl;
  std::cout << "Now type a number and then a string !" << std::endl;
  number = 0;
  string.clear();
  std::cin >> number;
  std::cin >> string;
  std::cout << "Your number is " << number << " and string is " << string << std::endl;
  std::cout << "Now we are back to raw mode. Play with keys and press CTRL_C to quit." << std::endl;
  Term::Key event;
  while((event = Term::read_event()) != Term::Key(Term::Key::CTRL_C))
  {
    if(event.isprint()) std::cout << "You typed :" << static_cast<char>(event) << std::endl;
    else
      std::cout << "You typed key :" << static_cast<std::uint32_t>(event) << std::endl;
  }
  return 0;
}
