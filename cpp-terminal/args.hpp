/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>
#include <vector>

namespace Term
{

class Arguments
{
public:
  Arguments();
  static std::size_t              argc();
  static std::vector<std::string> argv();
  std::string                     operator[](const std::size_t&) const;

private:
  static void                     parse();
  static std::vector<std::string> m_args;
  static bool                     m_parsed;
};

class Argc
{
public:
  Argc();
  operator unsigned int();
  operator unsigned int() const;
};

static const Arguments argv;
static const Argc      argc;

}  // namespace Term
