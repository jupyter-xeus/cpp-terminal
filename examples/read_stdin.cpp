#include "cpp-terminal/input.hpp"
#include "cpp-terminal/terminal.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main()
{
  Term::Terminal term(false, true, false);
  //std::string in{Term::read_stdin()};
  std::cout << "Input from stdin: " << Term::read_stdin() << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
}
