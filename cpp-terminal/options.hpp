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
    Default       = 0,
    Raw           = 1,
    Cooked        = -1,
    ClearScreen   = 2,
    NoClearScreen = -2,
    SignalKeys    = 3,
    NoSignalKeys  = -3,
    Cursor        = 4,
    NoCursor      = -4,
  };
  Options() = default;
  explicit Options(const std::vector<Option>& options);
  bool has(const Option& option);
  ~Options() {}
  std::vector<Option> getOptions();

private:
  std::vector<Option> m_Options;
};

typedef Term::Options::Option Option;

}  // namespace Term
