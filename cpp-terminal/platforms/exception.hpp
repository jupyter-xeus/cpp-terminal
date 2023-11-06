/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/exception.hpp"

#include <cstdint>

namespace Term
{

namespace Private
{

// Helper for windows errors
#if defined(_WIN32)
class WindowsException : public Term::Exception
{
public:
  WindowsException(const unsigned long& error, const std::string& context = std::string());
  virtual ~WindowsException() = default;

private:
  void build_what() override;
};

class WindowsError
{
public:
  WindowsError();
  ~WindowsError();
  std::int32_t  error() const;
  bool          check_value() const;
  WindowsError& check_if(const bool& ret);
  void          throw_exception(const std::string& str = std::string());

private:
  int  m_error{0};
  bool m_check_value{false};
};
#endif

class Errno
{
public:
  Errno(const Errno&) noexcept = default;
  Errno(Errno&&) noexcept      = default;
  Errno() noexcept;
  ~Errno() noexcept;
  Errno&        operator=(Errno&&) noexcept      = default;
  Errno&        operator=(const Errno&) noexcept = default;
  std::uint32_t error() const noexcept;
  bool          check_value() const noexcept;
  Errno&        check_if(const bool& ret) noexcept;
  void          throw_exception(const std::string& str = std::string()) const;

private:
  std::uint32_t m_errno{0};
  bool          m_check_value{false};
};

class ErrnoException : public Term::Exception
{
public:
  ErrnoException(const std::uint32_t& error, const std::string& context = std::string());
  ~ErrnoException() override;

private:
  void build_what() const noexcept override;
};

}  // namespace Private

}  // namespace Term
