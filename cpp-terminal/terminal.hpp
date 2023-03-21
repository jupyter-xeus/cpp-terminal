#pragma once

#include "cpp-terminal/terminfo.hpp"

#include <cstdio>

namespace Term
{

/* Note: the code that uses Terminal must be inside try/catch block, otherwise
 * the destructors will not be called when an exception happens and the
 * terminal will not be left in a good state. Terminal uses exceptions when
 * something goes wrong.
 */
class Terminal
{
public:
  Terminal(const bool& _clear_screen = false, const bool& _disable_signal_keys = true, const bool& _hide_cursor = false);
  ~Terminal();

private:
  FILE*          m_stdin{nullptr};
  FILE*          m_stdout{nullptr};
  FILE*          m_stderr{nullptr};
  void           store_and_restore();
  void           setRawMode();
  void           attachConsole();
  void           detachConsole();
  bool           clear_screen{};
  bool           disable_signal_keys{true};
  bool           hide_cursor{};
  bool           has_allocated_console{false};
  Term::Terminfo m_terminfo;
};

}  // namespace Term
