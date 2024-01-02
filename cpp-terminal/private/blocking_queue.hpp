/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

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
public:
  ~BlockingQueue()                                     = default;
  BlockingQueue()                                      = default;
  BlockingQueue(const BlockingQueue& other)            = delete;
  BlockingQueue(BlockingQueue&& other)                 = delete;
  BlockingQueue& operator=(const BlockingQueue& other) = delete;
  BlockingQueue& operator=(BlockingQueue&& other)      = delete;
  Term::Event    pop();
  void           push(const Term::Event& value, const std::size_t& occurrence = 1);
  void           push(const Term::Event&& value, const std::size_t& occurrence = 1);
  bool           empty();
  std::size_t    size();
  void           wait_for_events(std::unique_lock<std::mutex>& lock);

private:
  std::mutex              m_mutex;
  std::queue<Term::Event> m_queue;
  std::condition_variable m_cv;
};

}  // namespace Private
}  // namespace Term
