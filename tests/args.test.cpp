#define DOCTEST_CONFIG_IMPLEMENT
#include <cpp-terminal/args.hpp>
#include <doctest/doctest.h>
#include <iostream>

int    argc2;
char** argv2;

int main(int argc, char** argv)
{
  argc2 = argc;
  argv2 = argv;
  doctest::Context context;
  context.setOption("no-breaks", true);
  int res = context.run();
  if(context.shouldExit()) return res;
  context.clearFilters();
  return res;
}

TEST_CASE("argc")
{
  std::cout << "argc : " << argc2 << " Term::argc :" << Term::argc << std::endl;
  CHECK(Term::argc == argc2);
}

TEST_CASE("argc arithmetic")
{
  int  dumb{static_cast<int>(Term::argc)};
  bool result;
  // ==
  result = (dumb == Term::argc);
  CHECK(result == true);
  result = (Term::argc == dumb);
  CHECK(result == true);
  // >=
  result = (dumb >= Term::argc);
  CHECK(result == true);
  result = (Term::argc >= dumb);
  CHECK(result == true);
  // <=
  result = (dumb <= Term::argc);
  CHECK(result == true);
  result = (Term::argc <= dumb);
  CHECK(result == true);
  // !=
  result = (dumb != Term::argc);
  CHECK(result == false);
  result = (Term::argc != dumb);
  CHECK(result == false);
  // >
  result = (dumb > Term::argc);
  CHECK(result == false);
  result = (Term::argc > dumb);
  CHECK(result == false);
  // <
  result = (dumb < Term::argc);
  CHECK(result == false);
  result = (Term::argc < dumb);
  CHECK(result == false);
}

TEST_CASE("argv")
{
  for(std::size_t i = 0; i != Term::argc; ++i)
  {
    std::cout << "argv[" << i << "] : " << std::string(argv2[i]) << " Term::argv[" << i << "] : " << Term::argv[i] << std::endl;
    CHECK(Term::argv[i] == std::string(argv2[i]));
  }
}
