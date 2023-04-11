#pragma once

#include <iosfwd>

namespace Term
{

class Terminal;
extern Terminal& terminal;

class TerminalInitializer
{
public:
  TerminalInitializer();
  ~TerminalInitializer();

private:
  static int m_counter;
};

static TerminalInitializer m_terminalInitializer;

extern std::ofstream& cout;
extern std::ofstream& cerr;
extern std::ofstream& clog;
extern std::ifstream& cin;

}  // namespace Term
