#include "cpp-terminal/io.hpp"

#include <iostream>

int main()
{
  std::cout << "Just including io.hpp activate \033[31mcolor\033[0m !" << std::endl;
  int i{0};
  std::cin >> i;
  return 0;
}
