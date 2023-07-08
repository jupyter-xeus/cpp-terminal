#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/platforms/file.hpp"
#include "doctest/doctest.h"
#include <cstdio>

#if defined(_WIN32)
  #include <windows.h>
#endif

TEST_CASE("Test platform/file.hpp")
{
  DWORD dwCount;
  WriteConsole(reinterpret_cast<HANDLE>(Term::Private::std_cout.getHandler()), "Good !\n", strlen("Good !\n"), &dwCount, NULL);
  std::fprintf(Term::Private::std_cout.file(), "Good !\n");
}
