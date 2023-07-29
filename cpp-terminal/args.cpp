#include "cpp-terminal/args.hpp"
namespace Term
{

Term::Argc::operator std::size_t() { return Term::Arguments::argc(); }

Term::Argc::operator std::size_t() const { return Term::Arguments::argc(); }

bool Term::Argc::operator==(const int& i) const { return i == Term::Arguments::argc(); }

bool Term::Argc::operator!=(const int& i) const { return !(i == Term::Arguments::argc()); }

bool Term::Argc::operator>(const int& i) const { return (Term::Arguments::argc() > i); }

bool Term::Argc::operator<(const int& i) const { return (Term::Arguments::argc() < i); }

bool Term::Argc::operator>=(const int& i) const { return !(Term::Arguments::argc() < i); }

bool Term::Argc::operator<=(const int& i) const { return !(Term::Arguments::argc() > i); }

std::string Term::Arguments::operator[](const std::size_t& i) const { return m_args[i]; }

}  // namespace Term
