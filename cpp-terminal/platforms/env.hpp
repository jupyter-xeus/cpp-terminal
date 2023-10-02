/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
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

/**
* @brief Value of an environment variables.
*
* @param env environment variable.
* @return std::pair<bool, std::string> with bool:true if the environment variable is set and std::string to the value of environment variable.
* @warning Internal use only.
*/
std::pair<bool, std::string> getenv(const std::string& env);

}  // namespace Private

}  // namespace Term
