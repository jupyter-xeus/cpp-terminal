/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/env.hpp"

#include "cpp-terminal/private/unicode.hpp"

std::pair<bool, std::string> Term::Private::getenv(const std::string& key)
{
#if defined(_WIN32)
  std::size_t size{0};
  _wgetenv_s(&size, nullptr, 0, Term::Private::to_wide(key).c_str());
  std::wstring ret;
  if(size == 0 || size > ret.max_size()) return {false, std::string()};
  ret.reserve(size);
  _wgetenv_s(&size, &ret[0], size, Term::Private::to_wide(key).c_str());
  return {true, Term::Private::to_narrow(ret)};
#else
  if(std::getenv(key.c_str()) != nullptr) { return {true, static_cast<std::string>(std::getenv(key.c_str()))}; }
  return {false, std::string()};
#endif
}
