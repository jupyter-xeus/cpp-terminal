#pragma once

#include "cpp-terminal/event.hpp"

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
  static void                         read_event();
  static Term::Event                  read_raw();
  static std::thread                  m_thread;
  static Term::Private::BlockingQueue m_events;
};

}  // namespace Private

}  // namespace Term
