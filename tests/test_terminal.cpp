/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/platforms/unicode.hpp"
#include "doctest/doctest.h"

TEST_CASE("UTF conversions")
{
  char32_t    codepoint = U'\u0001';
  std::string s{Term::Private::utf32_to_utf8(codepoint)};
  CHECK(s.size() == 1);
  CHECK(s[0] == '\x01');
  CHECK(s == (const char*)u8"\u0001");

  codepoint = U'\u0080';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 2);
  CHECK(s[0] == '\xC2');
  CHECK(s[1] == '\x80');
  CHECK(s == (const char*)u8"\u0080");

  codepoint = U'\u0800';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 3);
  CHECK(s[0] == '\xE0');
  CHECK(s[1] == '\xA0');
  CHECK(s[2] == '\x80');
  CHECK(s == (const char*)u8"\u0800");

  codepoint = U'\U0010FFFF';
  s         = Term::Private::utf32_to_utf8(codepoint);
  CHECK(s.size() == 4);
  CHECK(s[0] == '\xF4');
  CHECK(s[1] == '\x8F');
  CHECK(s[2] == '\xBF');
  CHECK(s[3] == '\xBF');
  CHECK(s == (const char*)u8"\U0010FFFF");
}
