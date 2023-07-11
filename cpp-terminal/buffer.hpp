#pragma once

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
  Buffer() = default;
  void             setType(const Term::Buffer::Type& type);
  std::streambuf*  setbuf(char* s, std::streamsize n) final;
  virtual int_type overflow(int_type c) final;
  virtual int      sync() final;
  std::streamsize xsputn (const char* s, std::streamsize n);
protected:
private:
  std::string        remplace(const int_type&);
  std::string        m_s;
  Term::Buffer::Type m_type{Term::Buffer::Type::LineBuffered};
};

}  // namespace Term
