/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

namespace Term
{

// Returns true if the standard input is attached to a terminal
bool is_stdin_a_tty();
// Returns true if the standard output is attached to a terminal
bool is_stdout_a_tty();
// Returns true if the standard error is attached to a terminal
bool is_stderr_a_tty();

}  // namespace Term
