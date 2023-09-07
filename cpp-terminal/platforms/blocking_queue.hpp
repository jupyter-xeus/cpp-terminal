#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Term
{

class Event;

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
  void        push(const Term::Event& value);
  void        push(Term::Event&& value);
  bool        empty();
  std::size_t size();
  void        wait_for_events(std::unique_lock<std::mutex>& lock);
};

}  // namespace Private
}  // namespace Term
