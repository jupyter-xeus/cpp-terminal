/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_USE_STD_HEADERS
#include "cpp-terminal/platforms/exception.hpp"

#include "doctest/doctest.h"

#include <iostream>

#if defined(_WIN32)
TEST_CASE("WindowsError")
{
  Term::Private::WindowsError error(1);
  std::string                 ret{error.what()};
  CHECK(error.code() == 1);
}
#endif
