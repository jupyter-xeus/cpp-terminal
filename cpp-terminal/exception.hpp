#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace Term
{

class Exception : public std::exception
{
public:
  Exception(const std::string& what) : m_what(what){};
  virtual const char* what() const noexcept override { return m_what.c_str(); }

private:
  std::string m_what;
};

}  // namespace Term
