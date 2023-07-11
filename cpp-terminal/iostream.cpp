#include "cpp-terminal/iostream.hpp"

#include "cpp-terminal/io.hpp"
#include "cpp-terminal/stream.hpp"

#include <new>

namespace Term
{
// Output
static char coutBuf[sizeof(Term::TOstream)];
TOstream&   cout = reinterpret_cast<Term::TOstream&>(coutBuf);
static char cerrBuf[sizeof(Term::TOstream)];
TOstream&   cerr = reinterpret_cast<Term::TOstream&>(cerrBuf);
static char clogBuf[sizeof(Term::TOstream)];
TOstream&   clog = reinterpret_cast<Term::TOstream&>(clogBuf);
// Input
static char cinBuf[sizeof(Term::TIstream)];
TIstream&   cin = reinterpret_cast<Term::TIstream&>(cinBuf);
}  // namespace Term */

int Term::StreamInitializer::m_counter{0};

void Term::StreamInitializer::init()
{
  if(m_counter++ == 0)
  {
    Term::m_terminalInitializer.init();
    new(&Term::cout) TOstream(Term::Buffer::Type::LineBuffered);
    new(&Term::cerr) TOstream(Term::Buffer::Type::LineBuffered);
    new(&Term::clog) TOstream(Term::Buffer::Type::Unbuffered);
    new(&Term::cin) TIstream(Term::Buffer::Type::LineBuffered);
  }
}

Term::StreamInitializer::StreamInitializer() { init(); }

Term::StreamInitializer::~StreamInitializer()
{
  if(--m_counter == 0)
  {
    (&Term::cout)->~TOstream();
    (&Term::cerr)->~TOstream();
    (&Term::clog)->~TOstream();
    (&Term::cin)->~TIstream();
  }
}
