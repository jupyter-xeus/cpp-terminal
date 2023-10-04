/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Term
{
namespace Private
{

void codepoint_to_utf8(std::string& s, const char32_t& c);

}  // namespace Private

}  // namespace Term

#undef TERM_CONSTEXPR
