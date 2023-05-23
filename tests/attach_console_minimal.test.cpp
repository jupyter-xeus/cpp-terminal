#include "cpp-terminal/color.hpp"
#include "cpp-terminal/io.hpp"

#include <iostream>

#ifdef _WIN32
  #include <windows.h>
int __stdcall WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR cmdline, int show)
#else
int main()
#endif
{
  std::cout << Term::color_fg(Term::Color::Name::Red) << "Hello world !" << color_fg(Term::Color::Name::Default) << std::endl;
  return 0;
}
