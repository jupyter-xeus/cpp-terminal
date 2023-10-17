/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT
#include <cpp-terminal/args.hpp>
#include <doctest/doctest.h>
#include <iostream>
#include <string>

#if !defined(__wasm__)

  #if defined(_WIN32)
using char_type = wchar_t;
  #else
using char_type = char;
  #endif

int         argc2;
char_type** argv2;

  #if defined(_WIN32)

    #include <windows.h>
std::string to_utf8(LPCWCH utf16Str)
{
  int         size_needed = WideCharToMultiByte(CP_UTF8, 0, utf16Str, -1, nullptr, 0, nullptr, nullptr);
  std::string ret(size_needed, '\0');
  WideCharToMultiByte(CP_UTF8, 0, utf16Str, static_cast<int>(wcslen(utf16Str)), &ret[0], size_needed, nullptr, nullptr);
  return ret.c_str();
}
  #else
std::string to_utf8(const std::string& ret) { return ret.c_str(); }
  #endif

  #if defined(_WIN32)
int wmain(int argc, char_type** argv)
  #else
int main(int argc, char_type** argv)
  #endif
{
  argc2 = argc;
  argv2 = argv;
  doctest::Context context;
  context.setOption("no-breaks", true);
  int res = context.run();
  if(context.shouldExit()) return res;
  context.clearFilters();
  return res;
}

TEST_CASE("argc")
{
  std::cout << "argc : " << argc2 << " Term::argc :" << Term::argc << std::endl;
  CHECK(Term::argc == argc2);
}

TEST_CASE("argc arithmetic")
{
  int  dumb{static_cast<int>(Term::argc)};
  bool result;
  // ==
  result = (dumb == Term::argc);
  CHECK(result == true);
  result = (Term::argc == dumb);
  CHECK(result == true);
  // >=
  result = (dumb >= Term::argc);
  CHECK(result == true);
  result = (Term::argc >= dumb);
  CHECK(result == true);
  // <=
  result = (dumb <= Term::argc);
  CHECK(result == true);
  result = (Term::argc <= dumb);
  CHECK(result == true);
  // !=
  result = (dumb != Term::argc);
  CHECK(result == false);
  result = (Term::argc != dumb);
  CHECK(result == false);
  // >
  result = (dumb > Term::argc);
  CHECK(result == false);
  result = (Term::argc > dumb);
  CHECK(result == false);
  // <
  result = (dumb < Term::argc);
  CHECK(result == false);
  result = (Term::argc < dumb);
  CHECK(result == false);
}

TEST_CASE("argv")
{
  for(std::size_t i = 0; i != Term::argc; ++i)
  {
  #if defined(_WIN32)
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::wcout << "argv[" << i << "] : " << std::wstring(argv2[i]);
    std::cout << " Term::argv[" << i << "] : " << Term::argv[i] << std::endl;
  #else
    std::cout << "argv[" << i << "] : " << std::string(argv2[i]) << " Term::argv[" << i << "] : " << Term::argv[i] << std::endl;
  #endif
    CHECK(Term::argv[i] == to_utf8(&argv2[i][0]));
  }
}

#endif
