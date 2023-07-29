#include "cpp-terminal/args.hpp"
namespace Term
{

Term::Argc::operator long unsigned int() { return static_cast<int>(Term::Arguments::argc()); }

Term::Argc::operator long unsigned int() const { return static_cast<int>(Term::Arguments::argc()); }

std::string Term::Arguments::operator[](const std::size_t& i) const { return m_args[i]; }

}  // namespace Term
