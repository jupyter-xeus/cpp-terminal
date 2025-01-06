/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/options.hpp"

#include <doctest/doctest.h>

TEST_CASE("Options initializer_list") { Term::Options options = {Term::Option::Raw, Term::Option::Cursor}; }

TEST_CASE("Options commutativity")
{
  Term::Options options        = {Term::Option::Raw, Term::Option::Cursor};
  Term::Options options_swaped = {Term::Option::Cursor, Term::Option::Raw};
  CHECK(options == options_swaped);
}

TEST_CASE("Options : suppress redundancy")
{
  Term::Options options         = {Term::Option::Raw, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Raw, Term::Option::Cursor, Term::Option::Raw};
  Term::Options options2        = {Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Raw, Term::Option::Cursor, Term::Option::Raw};
  Term::Options options_cleaned = {Term::Option::Cursor, Term::Option::Raw};
  CHECK(options == options_cleaned);
  CHECK(options2 == options_cleaned);
}

TEST_CASE("Options : suppress */anti*")
{
  Term::Options options1 = {Term::Option::Raw, Term::Option::Cooked, Term::Option::Cursor};
  Term::Options options2 = {Term::Option::Cursor, Term::Option::Cooked, Term::Option::Raw};
  Term::Options options  = {Term::Option::Cursor};
  CHECK(options == options1);
  CHECK(options == options2);
}

TEST_CASE("Options template") { Term::Options options(Term::Option::Raw, Term::Option::Cooked, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor, Term::Option::Cursor); }
