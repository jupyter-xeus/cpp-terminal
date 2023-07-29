#pragma once

#include <string>
#include <vector>

namespace Term
{

class Arguments
{
public:
  static std::size_t              argc();
  static std::vector<std::string> argv();

private:
  static void                     parse();
  static std::vector<std::string> m_args;
  static bool                     m_parsed;
};

class Argc
{
public:
  explicit operator int();
  explicit operator int() const;
};

static const Arguments argv;
static const Argc      argc;

}  // namespace Term
