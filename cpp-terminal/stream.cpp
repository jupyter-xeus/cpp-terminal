#include "cpp-terminal/stream.hpp"

#include "cpp-terminal/io.hpp"

#include <string>

std::string Term::TIstream::file()
{
#if defined(_WIN32)
  return "CONIN$";
#else
  return "/dev/tty";
#endif
}

Term::TIstream::TIstream(const Term::Buffer::Type& type, const std::size_t& size) : m_stream(std::ifstream(file()))
{
  switch(type)
  {
    case Term::Buffer::Type::Unbuffered:
    {
      m_stream.rdbuf()->pubsetbuf(nullptr, 0);
      break;
    }
    case Term::Buffer::Type::LineBuffered:
    {
      m_string.reserve(size);
      m_stream.rdbuf()->pubsetbuf(&m_string[0], size);
      break;
    }
    case Term::Buffer::Type::FullBuffered:
    {
      m_string.reserve(size);
      m_stream.rdbuf()->pubsetbuf(&m_string[0], size);
      break;
    }
  }
}

Term::TOstream::TOstream(const Term::Buffer::Type& type, const std::size_t& size) : m_stream(&m_buffer)
{
  m_buffer.setType(type);
  m_stream.rdbuf()->pubsetbuf(nullptr, size);
}
