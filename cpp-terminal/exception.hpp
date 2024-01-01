/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <exception>
#include <string>

namespace Term
{

class Exception : public std::exception
{
public:
  explicit Exception(const std::string& message) noexcept;
  Exception(const std::int64_t& code, const std::string& message) noexcept;
  Exception(const Exception&)            = default;
  Exception(Exception&&)                 = default;
  Exception& operator=(Exception&&)      = default;
  Exception& operator=(const Exception&) = default;

  const char*  what() const noexcept override;
  std::int64_t code() const noexcept;
  std::string  message() const noexcept;
  std::string  context() const noexcept;
  ~Exception() noexcept override = default;

protected:
  explicit Exception(const std::int64_t& code) noexcept;
  virtual void                       build_what() const noexcept;
  void                               setMessage(const std::string& message) noexcept;
  void                               setContext(const std::string& context) noexcept;
  void                               setWhat(const std::string& what) const noexcept;
  static const constexpr std::size_t m_maxSize{256};

private:
  std::int64_t        m_code{0};
  std::string         m_message;
  std::string         m_context;
  mutable std::string m_what;
};

}  // namespace Term
