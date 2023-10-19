/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/env.hpp"

#include "cpp-terminal/platforms/unicode.hpp"

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
  if(std::getenv(key.c_str()) != nullptr) return {true, static_cast<std::string>(std::getenv(key.c_str()))};
  else
    return {false, std::string()};
#endif
}
