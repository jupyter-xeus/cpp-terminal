/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <string>

namespace Term
{

namespace Private
{

//-------------------------------------------
// Convert form UTF-8 to wide.
// Only for Windows to use with *W functions.
//-------------------------------------------
#if defined(_WIN32)
std::wstring utf8_to_wide(const std::string& utf8);
#endif

//-------------------------------------------
// Convert form wide to UTF-8.
// Only for Windows to use with *W functions.
//-------------------------------------------
#if defined(_WIN32)
std::string wide_to_utf8(const std::wstring& utf16);
#endif

}  // namespace Private

}  // namespace Term
