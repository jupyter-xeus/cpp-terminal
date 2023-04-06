#pragma once

#include <cstdint>
#include <vector>

namespace Term
{

class Options
{
public:
  enum class Option : int
  {
    // Don't use 0!
    ClearScreen   = 1,
    NoClearScreen = -1,
    SignalKeys    = 2,
    NoSignalKeys  = -2,
    Cursor        = 3,
    NoCursor      = -3,
  };
  Options(const std::vector<Option>& options);
  bool has(const Option& option);
  ~Options() {}

private:
  std::vector<Option> m_Options;
};

typedef Term::Options::Option Option;

}  // namespace Term
