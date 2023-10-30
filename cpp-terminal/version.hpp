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

namespace Term
{
namespace Version
{
std::uint16_t major();   ///< Major version of cpp-terminal.
std::uint16_t minor();   ///< Minor version of cpp-terminal.
std::uint16_t patch();   ///< Patch version of cpp-terminal.
std::string   string();  ///< String containing the version of cpp-terminal ("Major.Minor.Patch").
}  // namespace Version

std::string homepage();  ///< Homepage of cpp-terminal.
}  // namespace Term
