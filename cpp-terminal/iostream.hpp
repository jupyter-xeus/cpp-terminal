/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
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

static const IOStreamInitializer IO_stream_initializer;  //NOLINT(cert-err58-cpp,fuchsia-statically-constructed-objects)

}  // namespace Term
