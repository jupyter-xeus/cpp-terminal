/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
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
  Term::Private::WindowsException error(1);
  std::string                     ret{error.what()};
  CHECK(error.code() == 1);
}
#endif

TEST_CASE("Exception")
{
  Term::Exception exception("MyException");
  CHECK(exception.message() == "MyException");
  CHECK(exception.code() == 0);

  Term::Exception exception2(2, "MyException2");
  CHECK(exception2.message() == "MyException2");
  CHECK(exception2.code() == 2);
}

TEST_CASE("ErrnoException")
{
  errno = E2BIG;
  Term::Private::ErrnoException exception(errno, "MyErrno");
  CHECK(exception.context() == "MyErrno");
  CHECK(exception.code() == E2BIG);
}
