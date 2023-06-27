#pragma once

#include <cstdint>

namespace Term
{

class Focus
{
public:
  enum class Type : std::int32_t
  {
    Out     = 0,
    In      = 1,
    Unknown = 2,
  };
  Focus() = default;
  explicit Focus(const Term::Focus::Type& type);
  Term::Focus::Type type();
  bool              isIn();
  bool              isOut();

private:
  Term::Focus::Type m_focus{Term::Focus::Type::Unknown};
};

}  // namespace Term
