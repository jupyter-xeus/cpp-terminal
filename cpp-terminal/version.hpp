/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include <cstdint>
#include <string>

namespace Term
{
namespace Version
{

///
///@brief Major version of cpp-terminal.
///
///@return std::uint16_t major version.
///
std::uint16_t major() noexcept;

///
///@brief Minor version of cpp-terminal.
///
///@return std::uint16_t minor version.
///
std::uint16_t minor() noexcept;

///
///@brief Patch version of cpp-terminal.
///
///@return std::uint16_t patch version.
///
std::uint16_t patch() noexcept;

///
///@brief String version of cpp-terminal.
///
///@return std::string version in the format "Major.Minor.Patch"
///
std::string string() noexcept;

}  // namespace Version

///
///@brief Homepage of cpp-terminal.
///
///@return std::string return the URL of the cpp-terminal project.
///
std::string homepage() noexcept;

}  // namespace Term
