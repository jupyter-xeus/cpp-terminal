/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/event.hpp"
#include "doctest/doctest.h"

TEST_CASE("Test Term::Screen")
{
  Term::Screen default_screen;
  CHECK(default_screen.rows() == 0);
  CHECK(default_screen.columns() == 0);
  CHECK(default_screen.empty() == true);
}

TEST_CASE("Test Term::Screen(5,10)")
{
  Term::Screen screen({Term::Rows(5), Term::Columns(10)});
  CHECK(screen.rows() == 5);
  CHECK(screen.columns() == 10);
  CHECK(screen.empty() == false);
}
