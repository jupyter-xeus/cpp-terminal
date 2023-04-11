#include "cpp-terminal/io.hpp"

#include "cpp-terminal/terminal.hpp"

#include <new>

namespace Term
{
template<typename T> class Container
{
private:
  alignas(T) char t_buff[sizeof(T)];
};
static Container<Term::Terminal> termbuf;
Terminal&                        terminal = reinterpret_cast<Term::Terminal&>(termbuf);
}  // namespace Term

int Term::TerminalInitializer::m_counter{0};

Term::TerminalInitializer::TerminalInitializer()
{
  if(m_counter++ == 0) new(&Term::terminal) Terminal();
}

Term::TerminalInitializer::~TerminalInitializer()
{
  if(--m_counter == 0) { (&Term::terminal)->~Terminal(); }
}
