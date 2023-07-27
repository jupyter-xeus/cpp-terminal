#pragma once

#include <cstddef>
#include <cstdint>
#include <streambuf>

namespace Term
{

class Buffer : public std::streambuf
{
public:
  enum class Type : std::uint8_t
  {
    Unbuffered,
    LineBuffered,
    FullBuffered,
  };
  explicit Buffer(const Term::Buffer::Type& type = Term::Buffer::Type::LineBuffered, const std::streamsize& size = BUFSIZ);
  virtual ~Buffer() final;

protected:
  virtual int_type underflow() final;
  virtual int_type overflow(int c = std::char_traits<Term::Buffer::char_type>::eof());
  virtual int                    sync() final;

private:
  void               setType(const Term::Buffer::Type& type);
  std::streambuf*    setbuf(char* s, std::streamsize n) final;
  std::string        m_buffer;
  Term::Buffer::Type m_type{Term::Buffer::Type::LineBuffered};
};

}  // namespace Term
