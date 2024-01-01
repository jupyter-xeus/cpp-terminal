
/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>

namespace Term
{
namespace Private
{

// utf16 is useless and wstring too so utf16 inside wstring is useless^2 but windows use it so define this functions to deal with it and less the user forget it.
#if defined(_WIN32)
std::string  to_narrow(const std::wstring& wstr);
std::wstring to_wide(const std::string& str);
#endif

///
///@brief Encode a codepoint using UTF-8 \b std::string .
///
///@param codepoint The codepoint ( \b char32_t ) on range [0,0x10FFFF] to convert.
///@param exception If \b true throw exception on error, otherwise change the out of range \b codepoint to "replacement character" \b � .
///@return \b std::string the UTF-8 value.
///@warning Internal use only.
///
std::string utf32_to_utf8(const char32_t& codepoint, const bool& exception = false);

///
///@brief Encode a \b std::u32string into UTF-8 \b std::string .
///
///@param str The \b std::u32string to convert.
///@param exception If \b true throw exception on error, otherwise change the out of range \b codepoint to "replacement character" \b � .
///@return \b std::string encoded in UTF-8.
///@warning Internal use only.
///
std::string utf32_to_utf8(const std::u32string& str, const bool& exception = false);

}  // namespace Private
}  // namespace Term
