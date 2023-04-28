#pragma once

#include "cpp-terminal/io.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <fstream>

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
  Terminal();
  ~Terminal();

  template<typename Arg1 = Term::Option, typename... Args> void setOptions(Arg1 arg1, Args... args)
  {
    m_options.set(arg1);
    setOptions(args...);
  }

  template<typename T> Terminal& operator<<(const T& dt)
  {
    clog << dt;
    return *this;
  }
  template<typename T> std::istream& operator>>(T& dt)
  {
    if(m_options.has(Option::Raw))
    {
      Options options = m_options;
      store_and_restore();
      this->cin >> dt;
      store_and_restore();
      m_options = options;
      applyOptions();
      return this->cin;
    }
    else
    {
      this->cin >> dt;
      return this->cin;
    }
  }
  Terminal& operator<<(std::ostream& (*fun)(std::ostream&))
  {
    clog << fun;
    return *this;
  }

private:
  void           setOptions();
  void           applyOptions();
  std::ofstream  cout;
  std::ofstream  cerr;
  std::ofstream  clog;
  std::ifstream  cin;
  void           store_and_restore();
  void           setRawMode();
  void           attachConsole();
  void           detachConsole();
  void           attachStreams();
  void           detachStreams();
  bool           has_allocated_console{false};
  Term::Terminfo m_terminfo;
  Term::Options  m_options;
};

// change the title of the terminal, only supported by a few terminals
std::string terminal_title(const std::string& title);
// clear the screen and the scroll-back buffer
std::string clear_buffer();

}  // namespace Term
