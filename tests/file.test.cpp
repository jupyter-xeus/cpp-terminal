#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/platforms/file.hpp"

#include "doctest/doctest.h"

#include <cstdio>

TEST_CASE("Test platform/file.hpp")
{
  Term::Private::std_cout.write("Good !\n");
  std::fprintf(Term::Private::std_cout.file(), "Good !\n");
}
