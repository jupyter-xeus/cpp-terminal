/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 Ondřej Čertík
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
  CHECK(event.type() == Term::Event::Type::Empty);
  Term::Event event2{};
  CHECK(event2.empty() == true);
  CHECK(event2.type() == Term::Event::Type::Empty);
}
