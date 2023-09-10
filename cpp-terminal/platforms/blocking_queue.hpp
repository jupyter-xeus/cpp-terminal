#pragma once

#include "cpp-terminal/event.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Term
{

namespace Private
{

class BlockingQueue
{
private:
  std::mutex              mutex_;
  std::queue<Term::Event> queue_;
  std::condition_variable cv;

public:
  Term::Event pop();
  void        push(const Term::Event& value, const std::size_t& occurence = 1);
  void        push(const Term::Event&& value, const std::size_t& occurence=1);
  bool        empty();
  std::size_t size();
  void        wait_for_events(std::unique_lock<std::mutex>& lock);
};

}  // namespace Private
}  // namespace Term
