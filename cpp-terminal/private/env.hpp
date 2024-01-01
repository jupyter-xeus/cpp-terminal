/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

// This header should be used only in files contained inside platforms folder !!!
#pragma once

#include <string>
#include <utility>

namespace Term
{

namespace Private
{

///
/// @brief Value of an environment variables.
///
/// @param env The environment variable.
/// @return \b std::pair<bool,std::string> with \b bool set to \b true if the environment variable is set and \b std::string set to the value of environment variable.
/// @warning Internal use only.
///
std::pair<bool, std::string> getenv(const std::string& env);

}  // namespace Private

}  // namespace Term
