/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/blocking_queue.hpp"

Term::Event Term::Private::BlockingQueue::pop()
{
  const std::lock_guard<std::mutex> lk(mutex_);
  Term::Event                       value = this->queue_.front();
  queue_.pop();
  return value;
}

void Term::Private::BlockingQueue::push(const Term::Event& value, const std::size_t& occurrence)
{
  for(std::size_t i = 0; i != occurrence; ++i)
  {
    const std::lock_guard<std::mutex> lk(mutex_);
    queue_.push(value);
    cv.notify_all();
  }
}

void Term::Private::BlockingQueue::push(const Term::Event&& value, const std::size_t& occurrence)
{
  for(std::size_t i = 0; i != occurrence; ++i)
  {
    const std::lock_guard<std::mutex> lk(mutex_);
    queue_.push(std::move(value));
    cv.notify_all();
  }
}

bool Term::Private::BlockingQueue::empty()
{
  const std::lock_guard<std::mutex> lk(mutex_);
  return queue_.empty();
}

std::size_t Term::Private::BlockingQueue::size()
{
  const std::lock_guard<std::mutex> lk(mutex_);
  return queue_.size();
}

void Term::Private::BlockingQueue::wait_for_events(std::unique_lock<std::mutex>& lock) { cv.wait(lock); }
