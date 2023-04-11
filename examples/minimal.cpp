#include "cpp-terminal/io.hpp"

#include <fstream>
#include <iostream>

int main()
{
  Term::cout << "This should be printed only in terminal" << std::endl;
  std::cout << "This should be printed in terminal if stdout is not redirected" << std::endl;
}
