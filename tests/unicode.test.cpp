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
  std::string in((const char*)(u8"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n"));  // Some multi-language charabia
  std::wstring out{Term::Private::to_wide(in)};
  CHECK(out == L"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n");
}
#endif
