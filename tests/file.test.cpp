#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/platforms/file.hpp"

#include "doctest/doctest.h"

#include <cstdio>

TEST_CASE("Test platform/file.hpp")
{
#if !defined(__MINGW32__)
  Term::Private::m_fileInitializer.init();
  Term::Private::out.write("Good !\n");
  std::fprintf(Term::Private::out.file(), "Good !\n");
#endif
}
