/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include <cctype>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/key.hpp"
#include "doctest/doctest.h"

TEST_CASE("isalnum")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalnum(static_cast<char>(i))) == Term::Key(i).isalnum()); }
}

TEST_CASE("isalpha")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isalpha(static_cast<char>(i))) == Term::Key(i).isalpha()); }
}

TEST_CASE("isblank")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isblank(static_cast<char>(i))) == Term::Key(i).isblank()); }
}

TEST_CASE("iscntrl")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(iscntrl(static_cast<char>(i))) == Term::Key(i).iscntrl()); }
}

TEST_CASE("isdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isdigit(static_cast<char>(i))) == Term::Key(i).isdigit()); }
}

TEST_CASE("isgraph")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isgraph(static_cast<char>(i))) == Term::Key(i).isgraph()); }
}

TEST_CASE("islower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(islower(static_cast<char>(i))) == Term::Key(i).islower()); }
}

TEST_CASE("isprint")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isprint(static_cast<char>(i))) == Term::Key(i).isprint()); }
}

TEST_CASE("ispunct")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(ispunct(static_cast<char>(i))) == Term::Key(i).ispunct()); }
}

TEST_CASE("isspace")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isspace(static_cast<char>(i))) == Term::Key(i).isspace()); }
}

TEST_CASE("isupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isupper(static_cast<char>(i))) == Term::Key(i).isupper()); }
}

TEST_CASE("isxdigit")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(static_cast<bool>(isxdigit(static_cast<char>(i))) == Term::Key(i).isxdigit()); }
}

TEST_CASE("tolower")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(tolower(static_cast<char>(i)) == Term::Key(i).tolower()); }
}

TEST_CASE("toupper")
{
  for(std::size_t i = 0; i != 127; ++i) { CHECK(toupper(static_cast<char>(i)) == Term::Key(i).toupper()); }
}

TEST_CASE("key")
{
  for(std::size_t i = 0; i != 255; ++i) { CHECK(Term::Key(Term::Key(i)) == i); }
}

TEST_CASE("test MetaKey arithmetic")
{
  Term::MetaKey MyCtrl(Term::MetaKey::Value::Ctrl);
  CHECK(MyCtrl.hasAlt() == false);
  CHECK(MyCtrl.hasCtrl() == true);

  CHECK(Term::MetaKey(Term::MetaKey::Value::Alt).hasAlt() == true);
  CHECK(Term::MetaKey(Term::MetaKey::Value::Alt).hasCtrl() == false);
  CHECK(Term::MetaKey(Term::MetaKey::Value::Ctrl).hasAlt() == false);
  CHECK(Term::MetaKey(Term::MetaKey::Value::Ctrl).hasCtrl() == true);

  CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Alt).hasAlt() == true);
  CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Alt).hasCtrl() == true);
  CHECK((Term::MetaKey::Value::Alt + Term::MetaKey::Value::Ctrl).hasAlt() == true);
  CHECK((Term::MetaKey::Value::Alt + Term::MetaKey::Value::Ctrl).hasCtrl() == true);

  CHECK((Term::MetaKey::Value::Alt + Term::MetaKey::Value::Alt) == Term::MetaKey::Value::Alt);
  CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Ctrl) == Term::MetaKey::Value::Ctrl);
}

TEST_CASE("test MetaKey+NoKey arithmetic")
{
  Term::Key empty = Term::Key::NoKey;
  CHECK(empty.empty() == true);
  CHECK((Term::MetaKey::Value::Ctrl + empty).empty() == true);
  CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Ctrl + empty).empty() == true);
  CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Alt + empty).empty() == true);
  CHECK((Term::MetaKey::Value::Alt + empty).empty() == true);
}

TEST_CASE("test Ctrl+Key arithmetic with CTRL_*")
{
  for(std::size_t i = 0; i != 31; ++i)
  {
    Term::Key key(i);
    CHECK(key.iscntrl() == true);  // Take care here we need iscntrl not isCTRL
    CHECK(key.hasAlt() == false);
    CHECK((Term::MetaKey::Value::Ctrl + key).iscntrl() == true);
    CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Ctrl + key).iscntrl() == true);
    CHECK((Term::MetaKey::Value::Ctrl + Term::MetaKey::Value::Ctrl + key).hasCtrlAll() == true);
    CHECK((Term::MetaKey::Value::Alt + key).iscntrl() == false);
    //std::cout<<static_cast<std::int32_t>(Term::MetaKey::Value::Alt +key)<<std::endl;
    CHECK((Term::MetaKey::Value::Alt + key).hasAlt() == true);
  }
}

TEST_CASE("test ALT+Key arithmetic")
{
  for(std::size_t i = 0; i != 255; ++i)
  {
    //std::cout<<"Begin"<<std::endl;
    Term::Key key(i);
    CHECK((Term::MetaKey::Value::Alt + key).hasAlt() == true);
    // std::cout<<static_cast<std::int32_t>(Term::MetaKey::Value::Alt+Term::MetaKey::Value::Ctrl+Term::Key(Term::Key::Null))<<std::endl;
    // std::cout<<"End"<<std::endl;
  }
}

TEST_CASE("test name()")
{
  for(std::size_t i = 0; i != 255; ++i)
  {
    Term::Key key(i);
    std::cout << "Value : " << i << " Name : " << key.name() << std::endl;
  }
}
