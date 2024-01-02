/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <streambuf>

namespace Term
{

class Buffer final : public std::streambuf
{
public:
  enum class Type : std::uint8_t
  {
    Unbuffered,
    LineBuffered,
    FullBuffered
  };
  explicit Buffer(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::streamsize& size = BUFSIZ);
  ~Buffer() override;
  Buffer(const Buffer&)            = delete;
  Buffer(Buffer&&)                 = delete;
  Buffer& operator=(Buffer&&)      = delete;
  Buffer& operator=(const Buffer&) = delete;

protected:
  int_type underflow() override;
  int_type overflow(int c = std::char_traits<Term::Buffer::char_type>::eof()) override;
  int      sync() override;

private:
  void               setType(const Term::Buffer::Type& type);
  std::streambuf*    setbuf(char* s, std::streamsize n) override;
  std::string        m_buffer;
  Term::Buffer::Type m_type{Term::Buffer::Type::LineBuffered};
};

}  // namespace Term
