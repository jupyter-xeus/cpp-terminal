/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
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

std::uint8_t utf8_decode_step(std::uint8_t state, std::uint8_t octet, std::uint32_t* cpp);

void codepoint_to_utf8(std::string& str, const char32_t& cha);

std::u32string utf8_to_utf32(const std::string& str);

std::string utf32_to_utf8(const std::u32string& str);

bool is_valid_utf8_code_unit(const std::string& str);

}  // namespace Private

}  // namespace Term
