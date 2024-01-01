/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/blocking_queue.hpp"

Term::Event Term::Private::BlockingQueue::pop()
{
  const std::lock_guard<std::mutex> lock(m_mutex);
  Term::Event                       value = this->m_queue.front();
  m_queue.pop();
  return value;
}

void Term::Private::BlockingQueue::push(const Term::Event& value, const std::size_t& occurrence)
{
  for(std::size_t i = 0; i != occurrence; ++i)
  {
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(value);
    m_cv.notify_all();
  }
}

void Term::Private::BlockingQueue::push(const Term::Event&& value, const std::size_t& occurrence)
{
  for(std::size_t i = 0; i != occurrence; ++i)
  {
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(value);
    m_cv.notify_all();
  }
}

bool Term::Private::BlockingQueue::empty()
{
  const std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.empty();
}

std::size_t Term::Private::BlockingQueue::size()
{
  const std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.size();
}

void Term::Private::BlockingQueue::wait_for_events(std::unique_lock<std::mutex>& lock) { m_cv.wait(lock); }
