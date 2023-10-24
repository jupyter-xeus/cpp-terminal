/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/terminal.hpp"

#include <cstddef>
#include <cstdint>
#include <streambuf>

namespace Term
{

class Buffer final : public std::streambuf
{
public:
  enum class StreamType : std::uint8_t
  {
    Cin      = 0,
    Cout     = 1,
    Clog     = 2,
    Terminal = 3,
  };

  enum class Type : std::uint8_t
  {
    Unbuffered,
    LineBuffered,
    FullBuffered
  };
  explicit Buffer(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::streamsize& size = BUFSIZ, const Term::Buffer::StreamType stream_type = Term::Buffer::StreamType::Terminal);
  ~Buffer() final;
  Buffer(const Buffer&)            = delete;
  Buffer& operator=(const Buffer&) = delete;
  Buffer(Buffer&&)                 = delete;
  Buffer& operator=(Buffer&&)      = delete;

protected:
  int_type underflow() final;
  int_type overflow(int c = std::char_traits<Term::Buffer::char_type>::eof()) final;
  int      sync() final;

private:
  void                     setType(const Term::Buffer::Type& type);
  std::streambuf*          setbuf(char* s, std::streamsize n) final;
  std::string              m_buffer;
  Term::Buffer::Type       m_type{Term::Buffer::Type::LineBuffered};
  Term::Buffer::StreamType m_streamType{Term::Buffer::StreamType::Terminal};
  std::string              read();
};

}  // namespace Term
