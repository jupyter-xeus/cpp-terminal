#include "cpp-terminal/buffer.hpp"

#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/terminal.hpp"
#include "options.hpp"

#include <iostream>

std::string Term::Buffer::remplace(const int_type& c)
{
#if defined(_WIN32)
  std::string ret;
  if(static_cast<char>(c) == '\n') ret = "\r\n";
  else
    ret.push_back(static_cast<char>(c));
  return ret;
#else
  std::string ret;
  ret.push_back(static_cast<char>(c));
  return ret;
#endif
}

int Term::Buffer::sync()
{
  int ret = Term::Private::out.write(m_buffer);
  m_buffer.clear();
  return ret;
}

Term::Buffer::Buffer(const Term::Buffer::Type& type, const std::streamsize& size)
{
  setType(type);
  switch(m_type)
  {
    case Type::Unbuffered: setbuf(nullptr, 0); break;
    case Type::LineBuffered:
    case Type::FullBuffered: setbuf(m_buffer.data(), size); break;
  }
}

void Term::Buffer::setType(const Term::Buffer::Type& type) { m_type = type; }

std::streambuf* Term::Buffer::setbuf(char* s, std::streamsize n)
{
  if(s != nullptr) m_buffer.reserve(n);
  return this;
}

Term::Buffer::int_type Term::Buffer::underflow()
{
  try
  {
    m_buffer.clear();
    if(terminal.getOptions().has(Option::Raw))
    {
      do {
        std::string ret{Term::Private::in.read()};
        m_buffer += ret;
        Term::Private::out.write(ret);
      } while(m_buffer.empty() || m_buffer.back() != '\r' || m_buffer.back() == '\n' || m_buffer.back() == std::char_traits<Term::Buffer::char_type>::eof());
      Term::Private::out.write('\n');
    }
    else
    {
      do {
        std::string ret{Term::Private::in.read()};
        m_buffer += ret;
      } while(m_buffer.empty());
    }
    setg(m_buffer.data(), m_buffer.data(), m_buffer.data() + m_buffer.size());
    return traits_type::to_int_type(m_buffer.at(0));
  }
  catch(...)
  {
    return traits_type::eof();
  }
}

Term::Buffer::int_type Term::Buffer::overflow(int c)
{
  if(c != std::char_traits<Term::Buffer::char_type>::eof())
  {
    switch(m_type)
    {
      case Type::Unbuffered:
      {
        Term::Private::out.write(remplace(c));
        break;
      }
      case Type::LineBuffered:
      {
        m_buffer += remplace(c);
        if(static_cast<char>(c) == '\n')
        {
          Term::Private::out.write(m_buffer);
          m_buffer.clear();
        }
        break;
      }
      case Type::FullBuffered:
      {
        if(m_buffer.size() >= m_buffer.capacity())
        {
          Term::Private::out.write(m_buffer);
          m_buffer.clear();
        }
        m_buffer += remplace(c);
        break;
      }
    }
  }
  return c;
}
