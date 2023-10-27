/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/exception.hpp"

#include <string>

Term::Exception::Exception(const std::string& message) : m_message(message) { build_what(); }

Term::Exception::Exception(const std::int64_t& code, const std::string& message) : m_code(code), m_message(message) { build_what(); }

Term::Exception::Exception(const std::int64_t& code) : m_code(code) {}

Term::Exception::~Exception() = default;

const char* Term::Exception::what() const noexcept { return m_what.c_str(); }

void Term::Exception::build_what()
{
  if(m_code == 0) { m_what = m_message; }
  else
    m_what = "error " + std::to_string(m_code) + ": " + m_message;
}

std::int64_t Term::Exception::code() const noexcept { return m_code; }

std::string Term::Exception::message() const noexcept { return m_message; }

std::string Term::Exception::context() const noexcept { return m_context; }
