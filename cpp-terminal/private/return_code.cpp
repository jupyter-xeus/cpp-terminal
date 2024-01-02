/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/return_code.hpp"

#include "cpp-terminal/private/env.hpp"

#include <cstdlib>
#include <string>
#include <utility>

std::uint16_t Term::returnCode() noexcept
{
  static std::uint16_t               code{EXIT_FAILURE};
  const std::pair<bool, std::string> returnCode{Private::getenv("CPP_TERMINAL_BADSTATE")};
  try
  {
    if(returnCode.first && (std::stoi(returnCode.second) != EXIT_SUCCESS)) { code = static_cast<std::uint16_t>(std::stoi(returnCode.second)); }
  }
  catch(...)
  {
    code = EXIT_FAILURE;
  }
  return code;
}
