/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
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
  explicit Exception(const std::string& message);
  Exception(const std::int64_t& code, const std::string& message);
  Exception(const Exception& exception) = default;
  const char*  what() const noexcept override;
  std::int64_t code() const noexcept;
  std::string  message() const noexcept;
  std::string  context() const noexcept;
  ~Exception() override;

protected:
  explicit Exception(const std::int64_t& code);
  virtual void build_what();
  std::int64_t m_code{0};
  std::string  m_message;
  std::string  m_context;
  std::string  m_what;
};

}  // namespace Term
