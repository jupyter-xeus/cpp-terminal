#pragma once
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminfo.hpp"

namespace Term
{

class Terminal
{
public:
  Terminal();
  ~Terminal();

  template<typename Arg1 = Term::Option, typename... Args> void setOptions(Arg1 arg1, Args... args)
  {
    m_options.set(arg1);
    setOptions(args...);
  }

  void setOptions(const Term::Options& options);

  Term::Options getOptions();

  void store_and_restore();
  Terminal(const Terminal& other)          = delete;
  Terminal& operator=(const Terminal& rhs) = delete;
  Terminal(Terminal&& other)               = delete;
  Terminal& operator=(Terminal&& rhs)      = delete;

private:
  void           setBadStateReturnCode();
  void           setOptions();
  void           applyOptions();
  void           setRawMode();
  void           attachConsole();
  void           detachConsole();
  bool           has_allocated_console{false};
  Term::Terminfo m_terminfo;
  Term::Options  m_options;
  std::uint8_t   m_badReturnCode{EXIT_FAILURE};
};

class TerminalInitializer
{
public:
  TerminalInitializer();
  void init();
  ~TerminalInitializer();
  TerminalInitializer(const TerminalInitializer& other)          = delete;
  TerminalInitializer& operator=(const TerminalInitializer& rhs) = delete;
  TerminalInitializer(TerminalInitializer&& other)               = delete;
  TerminalInitializer& operator=(TerminalInitializer&& rhs)      = delete;

private:
  static int m_counter;
};

static TerminalInitializer m_terminalInitializer;
extern Term::Terminal&     terminal;

// change the title of the terminal, only supported by a few terminals
std::string terminal_title(const std::string& title);
// clear the screen and the scroll-back buffer
std::string clear_buffer();

}  // namespace Term
