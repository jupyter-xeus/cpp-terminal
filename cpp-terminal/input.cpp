// Bug in some GCC
#if !defined(_GLIBCXX_USE_NANOSLEEP)
  #define _GLIBCXX_USE_NANOSLEEP
#endif

#include "cpp-terminal/input.hpp"

#include "cpp-terminal/event.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>

std::int32_t Term::read_key()
{
  Term::Key key;
  while((key = Platform::read_raw()).empty()) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
  return key;
}

// returns the whole input from STDIN as string
std::string Term::read_stdin()
{
  std::string file;
  char        c{'\0'};
  while(true)
  {
    c = Platform::read_raw_stdin();
    if(c == 0x04) return file;
    else { file.push_back(c); }
  }
}
