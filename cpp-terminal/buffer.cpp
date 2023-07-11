#include "cpp-terminal/buffer.hpp"

#include "cpp-terminal/platforms/file.hpp"


std::string        Term::Buffer::remplace(const int_type& c)
{
#if defined(_WIN32)
  std::string ret;
  if(static_cast<char>(c)=='\n')  ret="\r\n";
  else ret.push_back(static_cast<char>(c));
  return ret;
#else
  return m_s;
#endif
}

void Term::Buffer::setType(const Term::Buffer::Type& type) { m_type = type; }

std::streamsize Term::Buffer::xsputn(const char* s, std::streamsize n)
{

  if(n == 0 || s[0] == EOF) return 0;
  std::string ret;
  ret.reserve(n);
  int i{0};
  do
  {
    ret+= remplace(s[i]);
    if(m_type==Type::LineBuffered && (s[i]=='\n'||(m_s.size()+ret.size()>=m_s.capacity())))
    {
      Term::Private::out.write(m_s+ret);
      m_s.clear();
    }
    ++i;
  }
  while(i < n);
  switch(m_type)
  {
    case Type::Unbuffered:
    {
      Term::Private::out.write(ret);
      break;
    }
    case Type::FullBuffered:
    {
      if(m_s.size()+ret.size()>=m_s.capacity())
      {
        Term::Private::out.write(m_s+ret);
        m_s.clear();
      }
      else m_s+=ret;
      break;
    }
  }
  return n;
}

std::streambuf* Term::Buffer::setbuf(char* s, std::streamsize n)
{
  s = s;
  m_s.reserve(n);
  return this;
}

Term::Buffer::int_type Term::Buffer::overflow(int_type c)
{
  if(c != EOF)
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
        m_s+=remplace(c);
        if(static_cast<char>(c) == '\n')
        {
          Term::Private::out.write(m_s);
          m_s.clear();
        }
        break;
      }
      case Type::FullBuffered:
      {

        if(m_s.size() >= m_s.capacity())
        {
          Term::Private::out.write(m_s);
          m_s.clear();
        }
        m_s+=remplace(c);
        break;
      }
    }
  }
  return c;
}

int Term::Buffer::sync()
{
  int ret = Term::Private::out.write(m_s);
  m_s.clear();
  return ret;
}
