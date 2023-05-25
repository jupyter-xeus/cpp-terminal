#include <cctype>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/key.hpp"
#include "doctest/doctest.h"

TEST_CASE("isalnum")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalnum(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isalnum()); }
}

TEST_CASE("isalpha")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalpha(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isalpha()); }
}

TEST_CASE("isblank")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isblank(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isblank()); }
}

TEST_CASE("iscntrl")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(iscntrl(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).iscntrl()); }
}

TEST_CASE("isdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isdigit(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isdigit()); }
}

TEST_CASE("isgraph")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isgraph(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isgraph()); }
}

TEST_CASE("islower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(islower(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).islower()); }
}

TEST_CASE("isprint")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isprint(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isprint()); }
}

TEST_CASE("ispunct")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(ispunct(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).ispunct()); }
}

TEST_CASE("isspace")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isspace(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isspace()); }
}

TEST_CASE("isupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isupper(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isupper()); }
}

TEST_CASE("isxdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isxdigit(static_cast<char>(i))) == Term::Key(static_cast<Term::Key::Value>(i)).isxdigit()); }
}

TEST_CASE("tolower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(tolower(static_cast<char>(i)) == Term::Key(static_cast<Term::Key::Value>(i)).tolower()); }
}

TEST_CASE("toupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(toupper(static_cast<char>(i)) == Term::Key(static_cast<Term::Key::Value>(i)).toupper()); }
}
