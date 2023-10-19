/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>

namespace Term
{

class Exception : public std::exception
{
public:
  Exception(const std::string& what) : m_what(what) {}
  Exception(const std::int64_t& code, const std::string& what) : m_what(what), m_code(code) {}
  virtual const char* what() const noexcept override { return m_what.c_str(); }
  std::int64_t        code() const noexcept { return m_code; }
  virtual ~Exception() = default;

protected:
  std::string  m_what;
  std::int64_t m_code{0};
};

}  // namespace Term
