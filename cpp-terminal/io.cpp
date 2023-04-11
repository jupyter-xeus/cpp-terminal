#include "cpp-terminal/io.hpp"
#include "cpp-terminal/terminal.hpp"

#include <new>

namespace Term
{
template<typename T> class Container
{
private:
  alignas(T) std::byte t_buff[sizeof(T)];
};
static Container<Term::Terminal> termbuf;
Terminal&                  terminal = reinterpret_cast<Term::Terminal&>(termbuf);

int TerminalInitializer::m_counter{0};

TerminalInitializer::TerminalInitializer()
{
  if(m_counter++ == 0) new(&Term::terminal) Terminal();
}

TerminalInitializer::~TerminalInitializer()
{
  if(--m_counter == 0) (&Term::terminal)->~Terminal();
}

}  // namespace Term