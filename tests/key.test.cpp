#include <cctype>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/key.hpp"
#include "doctest/doctest.h"

TEST_CASE("isalnum")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalnum(static_cast<char>(i))) == Term::isalnum(Term::Key(i))); }
}

TEST_CASE("isalpha")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalpha(static_cast<char>(i))) == Term::isalpha(Term::Key(i))); }
}

TEST_CASE("isblank")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isblank(static_cast<char>(i))) == Term::isblank(Term::Key(i))); }
}

TEST_CASE("iscntrl")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(iscntrl(static_cast<char>(i))) == Term::iscntrl(Term::Key(i))); }
}

TEST_CASE("isdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isdigit(static_cast<char>(i))) == Term::isdigit(Term::Key(i))); }
}

TEST_CASE("isgraph")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isgraph(static_cast<char>(i))) == Term::isgraph(Term::Key(i))); }
}

TEST_CASE("islower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(islower(static_cast<char>(i))) == Term::islower(Term::Key(i))); }
}

TEST_CASE("isprint")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isprint(static_cast<char>(i))) == Term::isprint(Term::Key(i))); }
}

TEST_CASE("ispunct")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(ispunct(static_cast<char>(i))) == Term::ispunct(Term::Key(i))); }
}

TEST_CASE("isspace")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isspace(static_cast<char>(i))) == Term::isspace(Term::Key(i))); }
}

TEST_CASE("isupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isupper(static_cast<char>(i))) == Term::isupper(Term::Key(i))); }
}

TEST_CASE("isxdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isxdigit(static_cast<char>(i))) == Term::isxdigit(Term::Key(i))); }
}

TEST_CASE("tolower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(tolower(static_cast<char>(i)) == Term::tolower(Term::Key(i))); }
}

TEST_CASE("toupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(toupper(static_cast<char>(i)) == Term::toupper(Term::Key(i))); }
}

TEST_CASE("key")
{
  for(std::size_t i = 0; i != 255; ++i) { CHECK(Term::Key(Term::Key(i)) == i); }
}

TEST_CASE("test MetaKey arithmetic")
{
  Term::MetaKey MyCtrl(Term::MetaKey::Ctrl);
  CHECK(Term::hasAlt(MyCtrl) == false);
  CHECK(Term::hasCtrl(MyCtrl) == true);

  CHECK(Term::hasAlt(Term::MetaKey::Alt) == true);
  CHECK(Term::hasCtrl(Term::MetaKey::Alt) == false);
  CHECK(Term::hasAlt(Term::MetaKey::Ctrl) == false);
  CHECK(Term::hasCtrl(Term::MetaKey::Ctrl) == true);

  CHECK(Term::hasAlt((Term::MetaKey::Ctrl + Term::MetaKey::Alt)) == true);
  CHECK(Term::hasCtrl((Term::MetaKey::Ctrl + Term::MetaKey::Alt)) == true);
  CHECK(Term::hasAlt((Term::MetaKey::Alt + Term::MetaKey::Ctrl)) == true);
  CHECK(Term::hasCtrl((Term::MetaKey::Alt + Term::MetaKey::Ctrl)) == true);

  CHECK((Term::MetaKey::Alt + Term::MetaKey::Alt) == Term::MetaKey::Alt);
  CHECK((Term::MetaKey::Ctrl + Term::MetaKey::Ctrl) == Term::MetaKey::Ctrl);
}

TEST_CASE("test MetaKey+NoKey arithmetic")
{
  Term::Key empty = Term::Key::NoKey;
  CHECK(Term::empty(empty) == true);
  CHECK(Term::empty((Term::MetaKey::Ctrl + empty)) == true);
  CHECK(Term::empty((Term::MetaKey::Ctrl + Term::MetaKey::Ctrl + empty)) == true);
  CHECK(Term::empty((Term::MetaKey::Ctrl + Term::MetaKey::Alt + empty)) == true);
  CHECK(Term::empty((Term::MetaKey::Alt + empty)) == true);
}

TEST_CASE("test Ctrl+Key arithmetic with CTRL_*")
{
  for(std::size_t i = 0; i != 31; ++i)
  {
    Term::Key key(i);
    CHECK(Term::iscntrl(key) == true);  // Take care here we need iscntrl not isCTRL
    CHECK(Term::hasAlt(key) == false);
    CHECK(Term::iscntrl((Term::MetaKey::Ctrl + key)) == true);
    CHECK(Term::iscntrl((Term::MetaKey::Ctrl + Term::MetaKey::Ctrl + key)) == true);
    CHECK(Term::hasCtrlAll((Term::MetaKey::Ctrl + Term::MetaKey::Ctrl + key)) == true);
    CHECK(Term::iscntrl((Term::MetaKey::Alt + key)) == false);
    //std::cout<<static_cast<std::int32_t>(Term::MetaKey::Alt +key)<<std::endl;
    CHECK(Term::hasAlt((Term::MetaKey::Alt + key)) == true);
  }
}

TEST_CASE("test ALT+Key arithmetic")
{
  for(std::size_t i = 0; i != 255; ++i)
  {
    //std::cout<<"Begin"<<std::endl;
    Term::Key key(i);
    CHECK(Term::hasAlt((Term::MetaKey::Alt + key)) == true);
    // std::cout<<static_cast<std::int32_t>(Term::MetaKey::Alt+Term::MetaKey::Ctrl+Term::Key(Term::Key::Null))<<std::endl;
    // std::cout<<"End"<<std::endl;
  }
}

TEST_CASE("test name()")
{
  for(std::size_t i = 0; i != 255; ++i)
  {
    Term::Key key(i);
    std::cout << "Value : " << i << " Name : " << Term::name(key) << std::endl;
  }
}
