/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/stream.hpp"

Term::TIstream::TIstream(const Term::Buffer::Type& type, const std::streamsize& size) : m_buffer(type, size), m_stream(&m_buffer) {}

std::streambuf* Term::TIstream::rdbuf() const { return const_cast<Term::Buffer*>(&m_buffer); }

Term::TOstream::TOstream(const Term::Buffer::Type& type, const std::streamsize& size) : m_buffer(type, size), m_stream(&m_buffer) {}

Term::TOstream::~TOstream() { m_stream.flush(); }
