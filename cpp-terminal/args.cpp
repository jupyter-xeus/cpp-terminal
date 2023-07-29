#include "cpp-terminal/args.hpp"
namespace Term
{

Term::Argc::operator std::size_t() { return Term::Arguments::argc(); }

Term::Argc::operator std::size_t() const { return Term::Arguments::argc(); }

bool Term::Argc::operator==(const int& i) const { return static_cast<std::size_t>(i) == Term::Arguments::argc(); }

bool Term::Argc::operator!=(const int& i) const { return !(static_cast<std::size_t>(i) == Term::Arguments::argc()); }

bool Term::Argc::operator>(const int& i) const { return (Term::Arguments::argc() > static_cast<std::size_t>(i)); }

bool Term::Argc::operator<(const int& i) const { return (Term::Arguments::argc() < static_cast<std::size_t>(i)); }

bool Term::Argc::operator>=(const int& i) const { return !(Term::Arguments::argc() < static_cast<std::size_t>(i)); }

bool Term::Argc::operator<=(const int& i) const { return !(Term::Arguments::argc() > static_cast<std::size_t>(i)); }

std::string Term::Arguments::operator[](const std::size_t& i) const { return m_args[i]; }

}  // namespace Term
