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
  Exception(const std::int32_t& error_code, const std::string& what) : m_what(what), m_errorcode(error_code) { m_message = std::string("Error ") + std::to_string(m_errorcode) + " : " + m_what; }
  virtual const char* what() const noexcept override
  {
    if(m_errorcode == 0) return m_what.c_str();
    else
      return m_message.c_str();
  }
  virtual ~Exception() = default;

private:
  std::string  m_what;
  std::int32_t m_errorcode{0};
  std::string  m_message;
};

}  // namespace Term
