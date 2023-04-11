#include "cpp-terminal/options.hpp"

#include <algorithm>

Term::Options::Options(const std::vector<Option>& options) : m_Options(options) {}

std::vector<Term::Options::Option> Term::Options::getOptions() { return m_Options; }

// Return true is the option is set and not its opposite (* + No* = false)
bool Term::Options::has(const Option& option)
{
  if(std::find(m_Options.begin(), m_Options.end(), option) != m_Options.end() && std::find(m_Options.begin(), m_Options.end(), static_cast<Options::Option>(-1 * static_cast<std::int16_t>(option))) == m_Options.end()) return true;
  else
    return false;
}
