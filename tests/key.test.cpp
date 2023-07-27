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

TEST_CASE("key")
{
  for(std::size_t i=0;i!=255;++i) { CHECK(Term::Key(Term::Key::Value(i)) == i); }
}

TEST_CASE("test MetaKey arithmetic")
{

  Term::MetaKey MyCtrl(Term::MetaKey::Value::Ctrl);
  CHECK(MyCtrl.hasAlt()==false);
  CHECK(MyCtrl.hasCtrl()==true);

  CHECK(Term::Alt.hasAlt()==true);
  CHECK(Term::Alt.hasCtrl()==false);
  CHECK(Term::Ctrl.hasAlt()==false);
  CHECK(Term::Ctrl.hasCtrl()==true);

  CHECK((Term::Ctrl+Term::Alt).hasAlt()==true);
  CHECK((Term::Ctrl+Term::Alt).hasCtrl()==true);
  CHECK((Term::Alt+Term::Ctrl).hasAlt()==true);
  CHECK((Term::Alt+Term::Ctrl).hasCtrl()==true);

  CHECK((Term::Alt+Term::Alt)==Term::Alt);
  CHECK((Term::Ctrl+Term::Ctrl)==Term::Ctrl);
}

TEST_CASE("test MetaKey+NoKey arithmetic")
{
  Term::Key empty;
  CHECK(empty.empty()==true);
  CHECK((Term::Ctrl+empty).empty()==true);
  CHECK((Term::Ctrl+Term::Ctrl+empty).empty()==true);
  CHECK((Term::Ctrl+Term::Alt+empty).empty()==true);
  CHECK((Term::Alt+empty).empty()==true);
}

TEST_CASE("test Ctrl+Key arithmetic with CTRL_*")
{
  for(std::size_t i=0; i!=31;++i)
  {
   Term::Key key(static_cast<Term::Key::Value>(i));
    CHECK(key.iscntrl() == true); // Take care here we need iscntrl not isCTRL
    CHECK(key.hasAlt() == false);
    CHECK((Term::Ctrl + key).iscntrl() == true);
    CHECK((Term::Ctrl + Term::Ctrl + key).iscntrl() == true);
    CHECK((Term::Ctrl + Term::Ctrl + key).hasCtrlAll() == true);
    CHECK((Term::Alt + key).iscntrl() == false);
    //std::cout<<static_cast<std::int32_t>(Term::Alt +key)<<std::endl;
    CHECK((Term::Alt + key).hasAlt() == true);
  }
}

TEST_CASE("test ALT+Key arithmetic")
{
  for(std::size_t i=0; i!=255;++i)
  {
    //std::cout<<"Begin"<<std::endl;
    Term::Key key(static_cast<Term::Key::Value>(i));
    CHECK((Term::Alt + key).hasAlt() == true);
   // std::cout<<static_cast<std::int32_t>(Term::Alt+Term::Ctrl+Term::Key(Term::Key::Value::Null))<<std::endl;
   // std::cout<<"End"<<std::endl;
  }
}

TEST_CASE("test name()")
{
  for(std::size_t i=0; i!=255;++i)
  {
    Term::Key key(static_cast<Term::Key::Value>(i));
    std::cout<<"Value : "<<i<<" Name : "<<key.name()<<std::endl;
  }
}
