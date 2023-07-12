#include "cpp-terminal/stream.hpp"

Term::TIstream::TIstream(const Term::Buffer::Type& type, const std::size_t& size) : m_buffer(type, size), m_stream(&m_buffer) {}

std::streambuf* Term::TIstream::rdbuf() const { return const_cast<Term::Buffer*>(&m_buffer); }

Term::TOstream::TOstream(const Term::Buffer::Type& type, const std::size_t& size) : m_buffer(type, size), m_stream(&m_buffer) {}
