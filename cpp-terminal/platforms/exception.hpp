/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/exception.hpp"

namespace Term
{

namespace Private
{

// Helper for windows errors
#if defined(_WIN32)
class WindowsError : public Term::Exception
{
public:
  WindowsError(const unsigned long& error);
  virtual ~WindowsError() = default;
};
#endif

}  // namespace Private

}  // namespace Term
