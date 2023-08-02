#pragma once

#include "cpp-terminal/event.hpp"

#include <mutex>
#include <queue>
#include <thread>

namespace Term
{

namespace Private
{

template<typename T> class BlockingQueue;

class Input
{
public:
  Input();
  static void        startReading();
  static Term::Event getEvent();
  static Term::Event getEventBlocking();

private:
  static void                                      read_event();
  static Term::Event                               read_raw();
  static std::thread                               m_thread;
  static Term::Private::BlockingQueue<Term::Event> m_events;
};

}  // namespace Private

}  // namespace Term
