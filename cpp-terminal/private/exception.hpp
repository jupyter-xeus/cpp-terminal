/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/exception.hpp"

#include <cstdint>
#include <string>

namespace Term
{

namespace Private
{

#if defined(_WIN32)
class WindowsError
{
public:
  WindowsError(const WindowsError&)                     = default;
  WindowsError(WindowsError&&)                          = default;
  WindowsError() noexcept                               = default;
  virtual ~WindowsError() noexcept                      = default;
  WindowsError& operator=(WindowsError&&) noexcept      = default;
  WindowsError& operator=(const WindowsError&) noexcept = default;
  std::int64_t  error() const noexcept;
  bool          check_value() const noexcept;
  WindowsError& check_if(const bool& ret) noexcept;
  void          throw_exception(const std::string& str = std::string()) const;

private:
  std::int64_t m_error{0};
  bool         m_check_value{false};
};

class WindowsException : public Term::Exception
{
public:
  WindowsException(const std::int64_t& error, const std::string& context = std::string());
  ~WindowsException() override = default;

private:
  void build_what() const noexcept final;
};
#endif

class Errno
{
public:
  Errno(const Errno&) noexcept = default;
  Errno(Errno&&) noexcept      = default;
  Errno() noexcept;
  virtual ~Errno() noexcept;
  Errno&       operator=(Errno&&) noexcept      = default;
  Errno&       operator=(const Errno&) noexcept = default;
  std::int64_t error() const noexcept;
  bool         check_value() const noexcept;
  Errno&       check_if(const bool& ret) noexcept;
  void         throw_exception(const std::string& str = {}) const;

private:
  std::int64_t m_errno{0};
  bool         m_check_value{false};
};

class ErrnoException : public Term::Exception
{
public:
  ErrnoException(const ErrnoException&) = default;
  ErrnoException(ErrnoException&&)      = default;
  explicit ErrnoException(const std::int64_t& error, const std::string& context = {});
  ~ErrnoException() override                       = default;
  ErrnoException& operator=(ErrnoException&&)      = default;
  ErrnoException& operator=(const ErrnoException&) = default;

private:
  void build_what() const noexcept final;
};

enum class ExceptionDestination : std::uint8_t
{
  MessageBox = 0,
  StdErr,
};

void ExceptionHandler(const ExceptionDestination& destination = ExceptionDestination::StdErr) noexcept;

}  // namespace Private

}  // namespace Term
