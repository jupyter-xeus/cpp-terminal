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
namespace Private
{

class Sigwinch
{
public:
  static void registerSigwinch();
  static void blockSigwinch();
  static void unblockSigwinch();
  static bool isSigwinch(const int& fd = -1);
  static int  get();

private:
  static int m_fd;
};

}  // namespace Private
}  // namespace Term
