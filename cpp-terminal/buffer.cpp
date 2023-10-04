/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/buffer.hpp"

#include "cpp-terminal/options.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/terminal.hpp"

static std::string replace(const Term::Buffer::int_type& c)
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

static bool newline_sequence(const std::string& str)  //https://en.wikipedia.org/wiki/Newline
{
  if(str.back() == '\n' || str.back() == '\r' || str.back() == '\036' || str.back() == '\036' || str.back() == '\025') return true;
  else
    return false;
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
    case Type::FullBuffered: setbuf(&m_buffer[0], size); break;
  }
}

void Term::Buffer::setType(const Term::Buffer::Type& type) { m_type = type; }

std::streambuf* Term::Buffer::setbuf(char* s, std::streamsize n)
{
  if(s != nullptr) m_buffer.reserve(static_cast<std::size_t>(n));
  return this;
}

Term::Buffer::int_type Term::Buffer::underflow()
{
  try
  {
    //TODO Maybe use input function ?
    m_buffer.clear();
    if(terminal.getOptions().has(Option::Raw))
    {
      do {
        std::string ret{Term::Private::in.read()};
        if(!ret.empty())
        {
          if(ret[0] == '\x7f' || ret[0] == '\b')
          {
            Term::Private::out.write("\b \b");  //Backspace is DEL, CTRL+Backspace is Backspace '\b'
            if(!m_buffer.empty()) m_buffer.erase(m_buffer.size() - 1);
          }
          else if(ret[0] == '\033')
          {
            continue;  // For now if it's escape sequence do nothing
          }
          else if(ret[0] <= 31 && ret[0] != '\t' && ret[0] != '\b' && ret[0] != 127 && ret[0] != ' ' && ret[0] != '\n' && ret[0] != '\r') { continue; }
          else
          {
            Term::Private::out.write(ret);
            m_buffer += ret;
          }
        }
      } while(m_buffer.empty() || !newline_sequence(m_buffer));
      Term::Private::out.write('\n');
    }
    else
    {
      do {
        std::string ret{Term::Private::in.read()};
        m_buffer += ret;
      } while(m_buffer.empty());
    }
    setg(&m_buffer[0], &m_buffer[0], &m_buffer[0] + m_buffer.size());
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
        Term::Private::out.write(replace(c));
        break;
      }
      case Type::LineBuffered:
      {
        m_buffer += replace(c);
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
        m_buffer += replace(c);
        break;
      }
    }
  }
  return c;
}

Term::Buffer::~Buffer() { sync(); }
