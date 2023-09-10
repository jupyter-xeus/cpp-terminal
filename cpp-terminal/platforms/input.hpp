#pragma once

#include "cpp-terminal/event.hpp"

#include <thread>
#include <cstdint>

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
  static void                         read_event();
  static void                         read_raw();
  static void                         read_windows_key(const std::uint16_t& virtual_key_code, const std::uint32_t& control_key_state,const std::size_t& occurence);                        
  static std::thread                  m_thread;
  static Term::Private::BlockingQueue m_events;
};

}  // namespace Private

}  // namespace Term
