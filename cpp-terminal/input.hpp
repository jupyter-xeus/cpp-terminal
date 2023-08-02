#pragma once

#include "cpp-terminal/event.hpp"

#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace Term
{

namespace Platform
{

class Input
{
public:
  template<typename T> class BlockingQueue
  {
  private:
    std::mutex    mutex_;
    std::queue<T> queue_;

  public:
    T pop()
    {
      this->mutex_.lock();
      T value = this->queue_.front();
      this->queue_.pop();
      this->mutex_.unlock();
      return value;
    }

    void push(T value)
    {
      this->mutex_.lock();
      this->queue_.push(value);
      this->mutex_.unlock();
    }

    bool empty()
    {
      this->mutex_.lock();
      bool check = this->queue_.empty();
      this->mutex_.unlock();
      return check;
    }
    std::size_t size()
    {
      this->mutex_.lock();
      std::size_t check = this->queue_.size();
      this->mutex_.unlock();
      return check;
    }
  };
  Input();
  static void        startReading();
  static Term::Event getEvent();
  static Term::Event getEventBlocking();

private:
  static void                       read_event();
  // Add to the stack
  static Term::Event                read_raw();
  static std::thread                m_thread;
  static BlockingQueue<Term::Event> m_events;
};

}  // namespace Platform

Term::Event read_event();

}  // namespace Term
