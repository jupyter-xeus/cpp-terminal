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
  std::string                     operator[](const std::size_t&) const;

private:
  static void                     parse();
  static std::vector<std::string> m_args;
  static bool                     m_parsed;
};

class Argc
{
public:
  operator std::size_t();
  operator std::size_t() const;
  bool operator==(const int&) const;
  bool operator!=(const int&) const;
  bool operator>(const int&) const;
  bool operator<(const int&) const;
  bool operator>=(const int&) const;
  bool operator<=(const int&) const;
};

static const Arguments argv;
static const Argc      argc;

}  // namespace Term
