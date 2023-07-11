#pragma once

namespace Term
{

class Terminal;
extern Terminal& terminal;

class TerminalInitializer
{
public:
  TerminalInitializer();
  void init();
  ~TerminalInitializer();

private:
  static int m_counter;
};

static TerminalInitializer m_terminalInitializer;

}  // namespace Term
