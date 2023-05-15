#include <cstdio>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/platforms/file.hpp"
#include "doctest/doctest.h"

#if defined(_WIN32)
  #include <windows.h>
#endif

TEST_CASE("Test platform/file.hpp")
{
#if defined(_WIN32)
  DWORD dwCount;
  WriteConsole(reinterpret_cast<HANDLE>(Term::Private::std_cout.getHandler()), "Good !\n", strlen("Good !\n"), &dwCount, NULL);
#else
  std::fprintf(fdopen(Term::Private::std_cout.getHandler(), "w"), "Good !\n");
#endif
}
