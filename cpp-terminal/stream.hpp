#pragma once

#include "cpp-terminal/buffer.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/terminal.hpp"

#include <fstream>
#include <ostream>
#include <string>

namespace Term
{

class TIstream
{
public:
  TIstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::size_t& size = BUFSIZ);
  template<typename T> TIstream& operator>>(T& t)
  {
    Term::Options options = Term::terminal.getOptions();
    if(options.has(Option::Raw))
    {
      Term::terminal.store_and_restore();
      m_stream >> t;
      Term::terminal.store_and_restore();
      Term::terminal.setOptions(options);
    }
    else { m_stream >> t; }
    return *this;
  }

private:
  static std::string file();
  TIstream(const TIstream& other)          = delete;
  TIstream& operator=(const TIstream& rhs) = delete;
  TIstream(TIstream&& other)               = delete;
  TIstream&     operator=(TIstream&& rhs)  = delete;
  std::ifstream m_stream;
  std::string   m_string;
};

class TOstream
{
public:
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
  TOstream(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::size_t& size = BUFSIZ);

private:
  TOstream(const TOstream& other)          = delete;
  TOstream& operator=(const TOstream& rhs) = delete;
  TOstream(TOstream&& other)               = delete;
  TOstream&    operator=(TOstream&& rhs)   = delete;
  Term::Buffer m_buffer;
  std::ostream m_stream;
};

}  // namespace Term
