#pragma once

#include <string>
#include <vector>

namespace Term
{

class Arguments
{
public:
  Arguments();
  static std::size_t              argc();
  static std::vector<std::string> argv();
  std::string                     operator[](const std::size_t&) const;

private:
  static void                     parse();
  static std::vector<std::string> m_args;
  static bool                     m_parsed;
};

class Argc
{
public:
  Argc();
  operator long unsigned int();
  operator long unsigned int() const;
};

static const Arguments argv;
static const Argc      argc;

}  // namespace Term
