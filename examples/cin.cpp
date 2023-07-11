#include "cpp-terminal/input.hpp"
#include "cpp-terminal/iostream.hpp"

int main()
{
  Term::terminal.setOptions(Term::Option::Raw);
  Term::cout << "This should be printed" << std::flush << " only in terminal" << std::endl;
  Term::cout << "Now type a number and then a string !" << std::endl;
  int         number{0};
  std::string string;
  Term::cin >> number >> string;
  Term::cout << "Your number is " << number << " and string is " << string << std::endl;
  Term::cout << "Now we are back to raw mode. Play with keys and press CTRL_C to quit." << std::endl;
  Term::Key event;
  while((event = Term::read_event()) != Term::Key(Term::Key::CTRL_C))
  {
    if(event.isprint()) Term::cout << "You typed :" << static_cast<char>(event) << std::endl;
    else
      Term::cout << "You typed key :" << static_cast<std::uint32_t>(event) << std::endl;
  }
  return 0;
}
