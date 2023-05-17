#include "cpp-terminal/platforms/env.hpp"

std::pair<bool, std::string> Term::Private::getenv(const std::string& env)
{
#ifdef _WIN32
  std::size_t requiredSize{0};
  getenv_s(&requiredSize, nullptr, 0, env.c_str());
  if(requiredSize == 0) return {false, std::string()};
  std::string ret;
  ret.reserve(requiredSize * sizeof(char));
  getenv_s(&requiredSize, &ret[0], requiredSize, env.c_str());
  return {true, ret};
#else
  if(std::getenv(env.c_str()) != nullptr) return {true, static_cast<std::string>(std::getenv(env.c_str()))};
  else
    return {false, std::string()};
#endif
}
