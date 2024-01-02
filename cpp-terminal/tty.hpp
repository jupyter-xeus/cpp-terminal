/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

namespace Term
{

///
/// @brief Check if \b stdin is a \b tty.
///
/// @return true : \b stdin is a \b tty.
/// @return false : \b stdin is not a \b tty.
///
bool is_stdin_a_tty();

///
/// @brief Check if \b stdout is a \b tty.
///
/// @return true : \b stdout is a \b tty.
/// @return false : \b stdout is not a \b tty.
///
bool is_stdout_a_tty();

///
/// @brief Check if \b stderr is a \b tty.
///
/// @return true : \b stderr is a \b tty.
/// @return false : \b stderr is not a \b tty.
///
bool is_stderr_a_tty();

}  // namespace Term
