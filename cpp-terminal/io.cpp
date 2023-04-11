#include "cpp-terminal/io.hpp"

#include "cpp-terminal/terminal.hpp"

#include <new>

namespace Term
{
static char termbuf[sizeof(Term::Terminal)];
Terminal&   terminal = reinterpret_cast<Term::Terminal&>(termbuf);
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
