#include "cpp-terminal/args.hpp"

#include <iostream>

int main()
{
  std::cout<<"argc : "<<Term::argc<<std::endl;
  for(std::size_t i=0;i!=Term::argc;++i)
  {
    std::cout<<"argv["<<i<<"] : *"<<Term::argv[i]<<"*"<<std::endl;
  }
  return 0;
}
