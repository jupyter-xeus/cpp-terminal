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

std::pair<bool, std::string> Term::Private::getenv(const std::string& env)
{
#if defined(_WIN32)
  std::size_t requiredSize{0};
  _wgetenv_s(&requiredSize, nullptr, 0, Term::Private::utf8_to_wide(env).c_str());
  std::wstring ret;
  if(requiredSize == 0 || requiredSize > ret.max_size()) return {false, std::string()};
  ret.reserve(requiredSize);
  _wgetenv_s(&requiredSize, &ret[0], requiredSize, Term::Private::utf8_to_wide(env).c_str());
  return {true, Term::Private::wide_to_utf8(ret)};
#else
  if(std::getenv(env.c_str()) != nullptr) return {true, static_cast<std::string>(std::getenv(env.c_str()))};
  else
    return {false, std::string()};
#endif
}
