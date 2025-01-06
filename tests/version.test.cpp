/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/version.hpp"

#include "doctest/doctest.h"

TEST_CASE("cpp-terminal version")
{
  CHECK(Term::Version::major() > 0);
  CHECK(Term::Version::minor() >= 0);
  CHECK(Term::Version::patch() >= 0);
  CHECK(Term::Version::string() == std::to_string(Term::Version::major()) + "." + std::to_string(Term::Version::minor()) + "." + std::to_string(Term::Version::patch()));
  CHECK(Term::homepage() == "https://github.com/jupyter-xeus/cpp-terminal");
}
