#pragma once

#include "cpp-terminal/buffer.hpp"
#include "cpp-terminal/terminal.hpp"

#include <istream>
#include <ostream>

namespace Term
{

class TIstream
{
public:
  TIstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::size_t& size = BUFSIZ);
  template<typename T> TIstream& operator>>(T& t)
  {
    /*Term::Options options = Term::terminal.getOptions();
    if(options.has(Option::Raw))
    {
      Term::terminal.store_and_restore();
      m_stream >> t;
      Term::terminal.store_and_restore();
      Term::terminal.setOptions(options);
    }
    else {*/
    m_stream >> t; /* }    */
    return *this;
  }
  TIstream(const TIstream&)            = delete;
  TIstream& operator=(const TIstream&) = delete;
  TIstream(TIstream&& other)           = delete;
  TIstream& operator=(TIstream&&)      = delete;

private:
  Term::Buffer m_buffer;
  std::istream m_stream;
};

class TOstream
{
public:
  TOstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::size_t& size = BUFSIZ);
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
