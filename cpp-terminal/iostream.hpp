/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/iostream_initializer.hpp"
#include "cpp-terminal/stream.hpp"

namespace Term
{

extern TIstream& cin;
extern TOstream& cout;
extern TOstream& cerr;
extern TOstream& clog;

static StreamInitializer m_streams;

}  // namespace Term
