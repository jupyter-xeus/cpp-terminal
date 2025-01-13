/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/exception.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/private/unicode.hpp"
#include "doctest/doctest.h"

#include <string>

#if defined(_WIN32)
TEST_CASE("to_narrow")
{
  std::wstring in(L"Hello World; 你好; おはようございます; בוקר טוב");
  std::string  out{Term::Private::to_narrow(in)};
  CHECK(out == (const char*)(u8"Hello World; 你好; おはようございます; בוקר טוב"));
}
#endif

#if defined(_WIN32)
TEST_CASE("to_wide")
{
  std::string  in((const char*)(u8"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n"));  // Some multi-language charabia
  std::wstring out{Term::Private::to_wide(in)};
  CHECK(out == L"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n");
}
#endif

TEST_CASE("utf32 to utf8")
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

  std::uint32_t out_of_range{0x110000};
  s = Term::Private::utf32_to_utf8(out_of_range, false);
  CHECK(s.size() == 3);
  CHECK(s[0] == '\xEF');
  CHECK(s[1] == '\xBF');
  CHECK(s[2] == '\xBD');
  //CHECK(s == (const char*)u8"\xEF\xBF\xBD"); // FIXME windows

  try
  {
    s = Term::Private::utf32_to_utf8(out_of_range, true);
  }
  catch(const Term::Exception& ex)
  {
    CHECK(ex.code() == 0);
    CHECK(std::string(ex.what()) == "Invalid UTF32 codepoint.");
  }
}
