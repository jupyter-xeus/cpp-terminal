// Bug in some GCC
#if !defined(_GLIBCXX_USE_NANOSLEEP)
  #define _GLIBCXX_USE_NANOSLEEP
#endif

#include "cpp-terminal/input.hpp"

#include "cpp-terminal/event.hpp"

#include <chrono>
#include <thread>
#include <type_traits>

Term::Event Term::read_event()
{
  Term::Event event;
  while((event = Platform::read_raw()).empty()) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
  return event;
}
