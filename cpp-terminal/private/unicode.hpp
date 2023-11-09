
/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
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

// utf16 is useless and wstring too so utf16 inside wstring is useless^2 but windows use it so define this functions to deal with it and less the user forget it.
#if defined(_WIN32)
std::string  to_narrow(const std::wstring& wstr);
std::wstring to_wide(const std::string& str);
#endif
}  // namespace Private
}  // namespace Term
