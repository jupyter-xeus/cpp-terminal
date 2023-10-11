/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/event.hpp"
#include "doctest/doctest.h"

TEST_CASE("default Event")
{
  Term::Event event;
  CHECK(event.empty() == true);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event.get_if_focus() == nullptr);
  CHECK(event.get_if_cursor() == nullptr);
  CHECK(event.get_if_key() == nullptr);
  CHECK(event.get_if_mouse() == nullptr);
  CHECK(event.get_if_copy_paste() == nullptr);
  CHECK(event.type() == Term::Event::Type::Empty);
  const Term::Event event2 = event;
  CHECK(event2.empty() == true);
  CHECK(event2.get_if_screen() == nullptr);
  CHECK(event2.get_if_focus() == nullptr);
  CHECK(event2.get_if_cursor() == nullptr);
  CHECK(event2.get_if_key() == nullptr);
  CHECK(event2.get_if_mouse() == nullptr);
  CHECK(event2.get_if_copy_paste() == nullptr);
  CHECK(event2.type() == Term::Event::Type::Empty);
}

TEST_CASE("Event with Screen")
{
  Term::Event event(Term::Screen(4, 2));
  CHECK(event.empty() == false);
  CHECK(*event.get_if_screen() == Term::Screen(4, 2));
  CHECK(event.get_if_focus() == nullptr);
  CHECK(event.get_if_cursor() == nullptr);
  CHECK(event.get_if_key() == nullptr);
  CHECK(event.get_if_mouse() == nullptr);
  CHECK(event.get_if_copy_paste() == nullptr);
  CHECK(event.type() == Term::Event::Type::Screen);
  const Term::Event event2 = event;
  CHECK(event2.empty() == false);
  CHECK(*event.get_if_screen() == Term::Screen(4, 2));
  CHECK(event2.get_if_focus() == nullptr);
  CHECK(event2.get_if_cursor() == nullptr);
  CHECK(event2.get_if_key() == nullptr);
  CHECK(event2.get_if_mouse() == nullptr);
  CHECK(event2.get_if_copy_paste() == nullptr);
  CHECK(event2.type() == Term::Event::Type::Screen);
}

TEST_CASE("Event with Focus")
{
  Term::Focus focus(Term::Focus::Type::Out);
  Term::Event event(focus);
  CHECK(event.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(*event.get_if_focus() == focus);
  CHECK(event.get_if_cursor() == nullptr);
  CHECK(event.get_if_key() == nullptr);
  CHECK(event.get_if_mouse() == nullptr);
  CHECK(event.get_if_copy_paste() == nullptr);
  CHECK(event.type() == Term::Event::Type::Focus);
  const Term::Event event2 = event;
  CHECK(event2.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(*event2.get_if_focus() == focus);
  CHECK(event2.get_if_cursor() == nullptr);
  CHECK(event2.get_if_key() == nullptr);
  CHECK(event2.get_if_mouse() == nullptr);
  CHECK(event2.get_if_copy_paste() == nullptr);
  CHECK(event2.type() == Term::Event::Type::Focus);
}

TEST_CASE("Event with Cursor")
{
  Term::Cursor cursor(1, 5);
  Term::Event  event(cursor);
  CHECK(event.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event.get_if_focus() == nullptr);
  CHECK(*event.get_if_cursor() == Term::Cursor(1, 5));
  CHECK(event.get_if_key() == nullptr);
  CHECK(event.get_if_mouse() == nullptr);
  CHECK(event.get_if_copy_paste() == nullptr);
  CHECK(event.type() == Term::Event::Type::Cursor);
  const Term::Event event2 = event;
  CHECK(event2.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event2.get_if_focus() == nullptr);
  CHECK(*event2.get_if_cursor() == Term::Cursor(1, 5));
  CHECK(event2.get_if_key() == nullptr);
  CHECK(event2.get_if_mouse() == nullptr);
  CHECK(event2.get_if_copy_paste() == nullptr);
  CHECK(event2.type() == Term::Event::Type::Cursor);
}

TEST_CASE("Event with Key")
{
  for(std::size_t key = 0; key != 127; ++key)
  {
    Term::Event event(key);
    CHECK(event.empty() == false);
    CHECK(event.get_if_screen() == nullptr);
    CHECK(event.get_if_focus() == nullptr);
    CHECK(event.get_if_cursor() == nullptr);
    CHECK(*event.get_if_key() == static_cast<Term::Key>(key));
    CHECK(event.get_if_mouse() == nullptr);
    CHECK(event.get_if_copy_paste() == nullptr);
    CHECK(event.type() == Term::Event::Type::Key);
    const Term::Event event2 = event;
    CHECK(event2.empty() == false);
    CHECK(event2.get_if_screen() == nullptr);
    CHECK(event2.get_if_focus() == nullptr);
    CHECK(event2.get_if_cursor() == nullptr);
    CHECK(*event2.get_if_key() == static_cast<Term::Key>(key));
    CHECK(event2.get_if_mouse() == nullptr);
    CHECK(event2.get_if_copy_paste() == nullptr);
    CHECK(event2.type() == Term::Event::Type::Key);
  }
  for(std::size_t key = 0x10FFFF + 1; key != 0x10FFFF + 36; ++key)
  {
    Term::Event event(key);
    CHECK(event.empty() == false);
    CHECK(event.get_if_screen() == nullptr);
    CHECK(event.get_if_focus() == nullptr);
    CHECK(event.get_if_cursor() == nullptr);
    CHECK(*event.get_if_key() == static_cast<Term::Key>(key));
    CHECK(event.get_if_mouse() == nullptr);
    CHECK(event.get_if_copy_paste() == nullptr);
    CHECK(event.type() == Term::Event::Type::Key);
    const Term::Event event2 = event;
    CHECK(event2.empty() == false);
    CHECK(event2.get_if_screen() == nullptr);
    CHECK(event2.get_if_focus() == nullptr);
    CHECK(event2.get_if_cursor() == nullptr);
    CHECK(*event2.get_if_key() == static_cast<Term::Key>(key));
    CHECK(event2.get_if_mouse() == nullptr);
    CHECK(event2.get_if_copy_paste() == nullptr);
    CHECK(event2.type() == Term::Event::Type::Key);
  }
}

TEST_CASE("Event with Mouse")
{
  Term::Mouse mouse(Term::Button{Term::Button{Term::Button::Type::Left, Term::Button::Action::Pressed}}, 1, 5);
  Term::Event event(mouse);
  CHECK(event.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event.get_if_focus() == nullptr);
  CHECK(event.get_if_cursor() == nullptr);
  CHECK(event.get_if_key() == nullptr);
  CHECK(*event.get_if_mouse() == mouse);
  CHECK(event.get_if_copy_paste() == nullptr);
  CHECK(event.type() == Term::Event::Type::Mouse);
  const Term::Event event2 = event;
  CHECK(event2.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event2.get_if_focus() == nullptr);
  CHECK(event2.get_if_cursor() == nullptr);
  CHECK(event2.get_if_key() == nullptr);
  CHECK(*event2.get_if_mouse() == mouse);
  CHECK(event2.get_if_copy_paste() == nullptr);
  CHECK(event2.type() == Term::Event::Type::Mouse);
}

TEST_CASE("Event with CopyPaste")
{
  Term::Event event("toto");
  CHECK(event.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event.get_if_focus() == nullptr);
  CHECK(event.get_if_cursor() == nullptr);
  CHECK(event.get_if_key() == nullptr);
  CHECK(event.get_if_mouse() == nullptr);
  CHECK(*event.get_if_copy_paste() == "toto");
  CHECK(event.type() == Term::Event::Type::CopyPaste);
  const Term::Event event2 = event;
  CHECK(event2.empty() == false);
  CHECK(event.get_if_screen() == nullptr);
  CHECK(event2.get_if_focus() == nullptr);
  CHECK(event2.get_if_cursor() == nullptr);
  CHECK(event2.get_if_key() == nullptr);
  CHECK(event2.get_if_mouse() == nullptr);
  CHECK(*event2.get_if_copy_paste() == "toto");
  CHECK(event2.type() == Term::Event::Type::CopyPaste);
}
