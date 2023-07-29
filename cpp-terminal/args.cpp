#include "cpp-terminal/args.hpp"
namespace Term
{

Term::Argc::operator int() { return Term::Arguments::argc(); }

Term::Argc::operator int() const { return Term::Arguments::argc(); }

}  // namespace Term
