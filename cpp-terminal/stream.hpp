/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/buffer.hpp"

#include <istream>
#include <ostream>

namespace Term
{

class TIstream
{
public:
  explicit TIstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::streamsize& size = BUFSIZ);
  template<typename T> TIstream& operator>>(T& t)
  {
    m_stream >> t;
    return *this;
  }
  TIstream(const TIstream&)             = delete;
  TIstream& operator=(const TIstream&)  = delete;
  TIstream(TIstream&& other)            = delete;
  TIstream&       operator=(TIstream&&) = delete;
  std::streambuf* rdbuf() const;

private:
  Term::Buffer m_buffer;
  std::istream m_stream;
};

class TOstream
{
public:
  explicit TOstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::streamsize& size = BUFSIZ);
  ~TOstream();
  template<typename T> TOstream& operator<<(const T& t)
  {
    m_stream << t;
    return *this;
  }
  TOstream& operator<<(std::ostream& (*t)(std::ostream&))
  {
    m_stream << t;
    return *this;
  }
  TOstream(const TOstream&)            = delete;
  TOstream& operator=(const TOstream&) = delete;
  TOstream(TOstream&&)                 = delete;
  TOstream& operator=(TOstream&&)      = delete;

private:
  Term::Buffer m_buffer;
  std::ostream m_stream;
};

}  // namespace Term
