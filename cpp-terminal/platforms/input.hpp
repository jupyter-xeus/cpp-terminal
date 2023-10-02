/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/event.hpp"

#include <cstdint>
#include <thread>

namespace Term
{

namespace Private
{

class BlockingQueue;

class Input
{
public:
  Input();
  static void        startReading();
  static Term::Event getEvent();
  static Term::Event getEventBlocking();

private:
  static void read_event();
  static void read_raw();
#if defined(_WIN32)
  static void read_windows_key(const std::uint16_t& virtual_key_code, const std::uint32_t& control_key_state, const std::size_t& occurrence);
#endif
  static void                         init_thread();
  static std::thread                  m_thread;
  static Term::Private::BlockingQueue m_events;
  static int                          m_poll;  // for linux
};

}  // namespace Private

}  // namespace Term
