#include "cpp-terminal/io.hpp"

#include "cpp-terminal/terminal.hpp"

#include <fstream>
#include <iostream>
#include <new>

namespace Term
{
template<typename T> class Container
{
private:
  alignas(T) std::byte t_buff[sizeof(T)];
};
static Container<Term::Terminal> termbuf;
Terminal&                        terminal = reinterpret_cast<Term::Terminal&>(termbuf);
static Container<std::ifstream>  cinbuf;
std::ifstream&                   cin = reinterpret_cast<std::ifstream&>(cinbuf);
static Container<std::ofstream>  coutbuf;
std::ofstream&                   cout = reinterpret_cast<std::ofstream&>(coutbuf);
static Container<std::ofstream>  cerrbuf;
std::ofstream&                   cerr = reinterpret_cast<std::ofstream&>(cerrbuf);
static Container<std::ofstream>  clogbuf;
std::ofstream&                   clog = reinterpret_cast<std::ofstream&>(clogbuf);
}  // namespace Term

int Term::TerminalInitializer::m_counter{0};

Term::TerminalInitializer::TerminalInitializer()
{
  if(m_counter++ == 0)
  {
    new(&Term::cout) std::ofstream();
    new(&Term::cerr) std::ofstream();
    new(&Term::clog) std::ofstream();
    new(&Term::cin) std::ifstream();
#if defined(_WIN32)
    Term::cout.open("CONOUT$", std::ofstream::out | std::ofstream::trunc);
    Term::cerr.open("CONOUT$", std::ofstream::out | std::ofstream::trunc);
    Term::clog.rdbuf()->pubsetbuf(nullptr, 0);
    Term::clog.open("CONOUT$", std::ofstream::out | std::ofstream::trunc);
    Term::clog.rdbuf()->pubsetbuf(nullptr, 0);
    Term::cin.open("CONIN$", std::ofstream::in | std::ofstream::trunc);
#else
    Term::cout.open("/dev/tty", std::ofstream::out | std::ofstream::trunc);
    Term::cerr.open("/dev/tty", std::ofstream::out | std::ofstream::trunc);
    Term::clog.rdbuf()->pubsetbuf(nullptr, 0);
    Term::clog.open("/dev/tty", std::ofstream::out | std::ofstream::trunc);
    Term::clog.rdbuf()->pubsetbuf(nullptr, 0);
    Term::cin.open("/dev/tty", std::ofstream::in | std::ofstream::trunc);
#endif
    Term::cerr.clear();
    Term::cin.clear();
    new(&Term::terminal) Terminal();
  }
}

Term::TerminalInitializer::~TerminalInitializer()
{
  if(--m_counter == 0)
  {
    if(Term::cout.is_open())
    {
      Term::cout.flush();
      Term::cout.close();
    }
    if(Term::cerr.is_open())
    {
      Term::cerr.flush();
      Term::cerr.close();
    }
    if(Term::clog.is_open())
    {
      Term::clog.flush();
      Term::clog.close();
    }
    if(Term::cin.is_open()) { Term::cin.close(); }
    (&Term::terminal)->~Terminal();
  }
}
